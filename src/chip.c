/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Flavio Heleno <https://github.com/flavioheleno>              |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "defines.h"
#include "chip.h"
#include "pin.h"
#include "pins.h"
#include "gpio.h"
#include "gpio_arginfo.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_object_handlers.h"

// #include <errno.h>
#include <fcntl.h>
// #include <limits.h>
#include <linux/gpio.h>
// #include <poll.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <string.h>
#include <sys/ioctl.h>
// #include <sys/stat.h>
// #include <sys/sysmacros.h>
// #include <sys/types.h>
// #include <unistd.h>

/* zend_object wrapper to ensure GPIO Chip is handled properly */
typedef struct _chipObject {
  int v2; /* ABI v2 flag */
  int fd; /* chip file descriptor */
  unsigned int pinCount;
  zend_object zendObject;
} chipObject;

static zend_object_handlers chipObjectHandlers;

/********************************/
/* Extension's Internal Methods */
/********************************/

/* retrieve chipObject pointer from a zend_object pointer */
static inline chipObject *getChipObject(zend_object *obj) {
  return (chipObject *)((char *)(obj) - XtOffsetOf(chipObject, zendObject));
}

/* handle chipObject release */
static void chipFreeObject(zend_object *obj) {
  chipObject *self = getChipObject(obj);

  /* failure to retrieve chip instance (edge case?) */
  if (self == NULL) {
    return;
  }

  /* if the chip was open, close it */
  if (self->fd >= 0) {
    close(self->fd);
    self->fd = -1;
  }

  /* release self->zendObject */
  zend_object_std_dtor(&self->zendObject);
}

/* custom unset($inst->prop) handler */
static void unsetPropertyObjectHandler(zend_object *object, zend_string *offset, void **cache_slot) {
  zend_throw_error(NULL, "Cannot unset GPIO\\Chip::$%s property", ZSTR_VAL(offset));
}

/********************************/
/* Extension's External Methods */
/********************************/

zend_class_entry* registerChipClass(void) {
  zend_class_entry ce, *classEntry;

  INIT_CLASS_ENTRY(ce, "GPIO\\Chip", class_GPIO_Chip_methods);
  classEntry = zend_register_internal_class(&ce);
  /* Final class / Objects of this class may not have dynamic properties */
  classEntry->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
  /* intercept object creation to change object handlers */
  classEntry->create_object = chipCreateObject;
  /* disable serialization */
  classEntry->serialize = zend_class_serialize_deny;
  /* disable unserialization */
  classEntry->unserialize = zend_class_unserialize_deny;

  /* Class Constants */
  zend_declare_class_constant_long(classEntry, "ABI_ANY", sizeof("ABI_ANY") - 1, ABI_ANY);
  zend_declare_class_constant_long(classEntry, "ABI_V1", sizeof("ABI_V1") - 1, ABI_V1);
  zend_declare_class_constant_long(classEntry, "ABI_V2", sizeof("ABI_V2") - 1, ABI_V2);

  /* initialize chipObjectHandlers with standard object handlers */
  memcpy(&chipObjectHandlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

  /* disable object cloning */
  chipObjectHandlers.clone_obj = NULL;
  /* Handler for objects that cannot be meaningfully compared. */
  /* chipObjectHandlers.compare = zend_objects_not_comparable; */
  /* sets the memory offset between chipObject and zendObject inside struct _chipObject */
  chipObjectHandlers.offset = XtOffsetOf(chipObject, zendObject);
  /* hook the object release */
  chipObjectHandlers.free_obj = chipFreeObject;
  /* hook the object property unset */
  chipObjectHandlers.unset_property = unsetPropertyObjectHandler;

  zval propDefaultValue;
  zend_string *propName;
  /* default property value (undefined) */
  ZVAL_UNDEF(&propDefaultValue);

  /* private string $path */
  propName = zend_string_init("path", sizeof("path") - 1, false);
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_STRING)
  );
  zend_string_release(propName);

  /* private string $label */
  propName = zend_string_init("label", sizeof("label") - 1, false);
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_STRING)
  );
  zend_string_release(propName);

  /* private string $name */
  propName = zend_string_init("name", sizeof("name") - 1, false);
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_STRING)
  );
  zend_string_release(propName);

  /* private int $pinCount */
  propName = zend_string_init("pinCount", sizeof("pinCount") - 1, false);
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_LONG)
  );
  zend_string_release(propName);

  return classEntry;
}

zend_object *chipCreateObject(zend_class_entry *zceClass) {
  chipObject *self = zend_object_alloc(sizeof(chipObject), zceClass);
  self->v2 = 0;
  self->fd = -1;
  self->pinCount = 0;

  zend_object_std_init(&self->zendObject, zceClass);
  object_properties_init(&self->zendObject, zceClass);

  /* set object handlers */
  self->zendObject.handlers = &chipObjectHandlers;

  return &self->zendObject;
}

/********************************/
/* PHP Visible Methods          */
/********************************/

/* {{{ GPIO\Chip::__construct(string $path, int $abiVersion) */
PHP_METHOD(GPIO_Chip, __construct) {
  char *path;
  size_t pathLen;
  zend_long abiVersion = ABI_ANY;
  ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_STRING(path, pathLen)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(abiVersion)
  ZEND_PARSE_PARAMETERS_END();

  chipObject *self = getChipObject(Z_OBJ_P(ZEND_THIS));
  self->fd = open(path, O_RDWR | O_CLOEXEC | O_NONBLOCK);
  if (self->fd < 0) {
    zend_throw_exception_ex(zceException, errno, "Failed to open chip: %s", strerror(errno));

    RETURN_THROWS();
  }

  struct gpiochip_info info;
  memset(&info, 0, sizeof(info));
  if (ioctl(self->fd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
    close(self->fd);
    zend_throw_exception_ex(zceException, errno, "Failed to retrieve chip information: %s", strerror(errno));

    RETURN_THROWS();
  }

  self->pinCount = info.lines;

  if (abiVersion == ABI_ANY || abiVersion == ABI_V2) {
    /* probe ABIv2 availability */
    struct gpio_v2_line_info probe;
    memset(&probe, 0, sizeof(probe));
    self->v2 = 1;
    if (ioctl(self->fd, GPIO_V2_GET_LINEINFO_IOCTL, &probe) < 0) {
      if (abiVersion == ABI_V2) {
        close(self->fd);
        zend_throw_exception_ex(zceException, errno, "ABIv2 is not available: %s", strerror(errno));

        RETURN_THROWS();
      }

      self->v2 = 0;
    }
  }

  /* update class property with constructor argument value */
  zend_update_property_stringl(zceChip, Z_OBJ_P(ZEND_THIS), "path", sizeof("path") - 1, path, pathLen);

  /* update class properties with chip information */
  if (info.label[0] == 0) {
    zend_update_property_string(zceChip, Z_OBJ_P(ZEND_THIS), "label", sizeof("label") - 1, "unknown");
  } else {
    zend_update_property_string(zceChip, Z_OBJ_P(ZEND_THIS), "label", sizeof("label") - 1, info.label);
  }

  zend_update_property_string(zceChip, Z_OBJ_P(ZEND_THIS), "name", sizeof("name") - 1, info.name);
  zend_update_property_long(zceChip, Z_OBJ_P(ZEND_THIS), "pinCount", sizeof("pinCount") - 1, info.lines);
}
/* }}} */

/* {{{ GPIO\Chip::isAbiV2(): bool */
PHP_METHOD(GPIO_Chip, isAbiV2) {
  ZEND_PARSE_PARAMETERS_NONE();

  chipObject *self = getChipObject(Z_OBJ_P(ZEND_THIS));

  RETURN_BOOL(self->v2);
}
/* }}} */

/* {{{ GPIO\Chip::getAllPins(): Pins */
PHP_METHOD(GPIO_Chip, getAllPins) {
  ZEND_PARSE_PARAMETERS_NONE();

  chipObject *self = getChipObject(Z_OBJ_P(ZEND_THIS));

  unsigned int *offsets;
  offsets = (unsigned int *)emalloc(sizeof(unsigned int) * self->pinCount);
  memset(offsets, 0, sizeof(*offsets));
  for (unsigned int i = 0; i < self->pinCount; i++) {
    offsets[i] = i;
  }

  /* create a new GPIO\Pins instance */
  zend_object *pins = pinsCreateObject(zcePins);
  pinsSetData(pins, self->v2, self->fd, offsets, self->pinCount);

  /* update GPIO\Pins::$chip instance property with reference to $this */
  zend_update_property(zcePins, pins, "chip", sizeof("chip") - 1, ZEND_THIS);

  RETURN_OBJ(pins);
}
/* }}} */

/* {{{ GPIO\Chip::getPin(int $offset): Pin */
PHP_METHOD(GPIO_Chip, getPin) {
  zend_long offset;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(offset)
  ZEND_PARSE_PARAMETERS_END();

  chipObject *self = getChipObject(Z_OBJ_P(ZEND_THIS));

  if (offset < 0) {
    zend_throw_exception_ex(zceException, EINVAL, "Invalid offset, cannot be negative");

    RETURN_THROWS();
  }

  if (offset >= self->pinCount) {
    zend_throw_exception_ex(zceException, EINVAL, "Invalid offset, cannot be greater than the number of pins");

    RETURN_THROWS();
  }

  /* create a new GPIO\Pin instance */
  zend_object *pin = pinCreateObject(zcePin);
  pinSetData(pin, self->v2, self->fd, offset);

  /* update GPIO\Pin::$chip instance property with reference to $this */
  zend_update_property(zcePin, pin, "chip", sizeof("chip") - 1, ZEND_THIS);

  RETURN_OBJ(pin);
}
/* }}} */

/* {{{ GPIO\Chip::getPins(array $offsets): Pins */
PHP_METHOD(GPIO_Chip, getPins) {
  HashTable *offsetsHashTable;
  zval *entry;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ARRAY_HT(offsetsHashTable)
  ZEND_PARSE_PARAMETERS_END();

  unsigned int numOffsets = zend_hash_num_elements(offsetsHashTable);

  chipObject *self = getChipObject(Z_OBJ_P(ZEND_THIS));

  unsigned int *offsets;
  offsets = (unsigned int *)emalloc(sizeof(unsigned int) * numOffsets);
  memset(offsets, 0, sizeof(*offsets));
  unsigned int i = 0;
  ZEND_HASH_FOREACH_VAL(offsetsHashTable, entry) {
    long value = zval_get_long(entry);
    if (value < 0) {
      zend_throw_exception_ex(zceException, EINVAL, "Invalid offset value, cannot be negative (%d)", value);

      RETURN_THROWS();
    }

    if (value >= self->pinCount) {
      zend_throw_exception_ex(zceException, EINVAL, "Invalid offset value, cannot be greater than the number of pins (%d)", value);

      RETURN_THROWS();
    }

    offsets[i++] = (unsigned int)value;
  } ZEND_HASH_FOREACH_END();

  /* create a new GPIO\Pins instance */
  zend_object *pins = pinsCreateObject(zcePins);
  pinsSetData(pins, self->v2, self->fd, offsets, numOffsets);

  /* update GPIO\Pins::$chip instance property with reference to $this */
  zend_update_property(zcePins, pins, "chip", sizeof("chip") - 1, ZEND_THIS);

  RETURN_OBJ(pins);
}
/* }}} */

/* {{{ GPIO\Chip::getPinCount(): int */
PHP_METHOD(GPIO_Chip, getPinCount) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *pinCount = zend_read_property(zceChip, Z_OBJ_P(ZEND_THIS), "pinCount", sizeof("pinCount") - 1, true, &rv);

  RETURN_LONG(zval_get_long(pinCount));
}
/* }}} */

/* {{{ GPIO\Chip::getPath(): string */
PHP_METHOD(GPIO_Chip, getPath) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *path = zend_read_property(zceChip, Z_OBJ_P(ZEND_THIS), "path", sizeof("path") - 1, true, &rv);

  RETURN_STR(zval_get_string(path));
}
/* }}} */

/* {{{ GPIO\Chip::getLabel(): string */
PHP_METHOD(GPIO_Chip, getLabel) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *label = zend_read_property(zceChip, Z_OBJ_P(ZEND_THIS), "label", sizeof("label") - 1, true, &rv);

  RETURN_STR(zval_get_string(label));
}
/* }}} */

/* {{{ GPIO\Chip::getName(): string */
PHP_METHOD(GPIO_Chip, getName) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *name = zend_read_property(zceChip, Z_OBJ_P(ZEND_THIS), "name", sizeof("name") - 1, true, &rv);

  RETURN_STR(zval_get_string(name));
}
/* }}} */
