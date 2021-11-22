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

#include "pins.h"
#include "pin.h"
#include "gpio.h"
#include "gpio_arginfo.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_object_handlers.h"

// #include <errno.h>
// #include <fcntl.h>
// #include <limits.h>
// #include <linux/gpio.h>
// #include <poll.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/ioctl.h>
// #include <sys/stat.h>
// #include <sys/sysmacros.h>
// #include <sys/types.h>
// #include <unistd.h>

/* zend_object wrapper to ensure gpiod_line_lines is handled properly */
struct _pinsObject {
  int v2; /* ABI v2 flag */
  int cfd; /* chip file descriptor */
  unsigned int *offsets;
  unsigned int count;
  unsigned int curr;
  zend_object zendObject;
};

static zend_object_handlers pinsObjectHandlers;

/********************************/
/* Extension's Internal Methods */
/********************************/

/* retrieve a pinsObject pointer from a zend_object pointer */
static inline pinsObject *getPinsObject(zend_object *obj) {
  return (pinsObject *)((char *)(obj) - XtOffsetOf(pinsObject, zendObject));
}

/* handle pinsObject release */
static void pinsFreeObject(zend_object *obj) {
  pinsObject *self = getPinsObject(obj);

  /* failure to retrieve pins instance (edge case?) */
  if (self == NULL) {
    return;
  }

  /* if the offsets pointer was set, release it */
  if (self->offsets != NULL) {
    efree(self->offsets);
    self->offsets = NULL;
    self->count = 0;
    self->curr = 0;
  }

  /* release self->zendObject */
  zend_object_std_dtor(&self->zendObject);
}

/* custom __construct handler */
static zend_function *getConstructorObjectHandler(zend_object *obj) {
  zend_throw_error(NULL, "Cannot directly construct GPIO\\Pins");

  return NULL;
}

/* custom unset($inst->prop) handler */
static void unsetPropertyObjectHandler(zend_object *object, zend_string *offset, void **cache_slot) {
  zend_throw_error(NULL, "Cannot unset GPIO\\Pins::$%s property", ZSTR_VAL(offset));
}

/********************************/
/* Extension's External Methods */
/********************************/

zend_class_entry* registerPinsClass(void) {
  zend_class_entry ce, *classEntry;

  INIT_CLASS_ENTRY(ce, "GPIO\\Pins", class_GPIO_Pins_methods);
  classEntry = zend_register_internal_class(&ce);
  /* Final class / Objects of this class may not have dynamic properties */
  classEntry->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
  /* intercept object creation to change object handlers */
  classEntry->create_object = pinsCreateObject;
  /* disable serialization */
  classEntry->serialize = zend_class_serialize_deny;
  /* disable unserialization */
  classEntry->unserialize = zend_class_unserialize_deny;

  /* initialize pinsObjectHandlers with standard object handlers */
  memcpy(&pinsObjectHandlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

  /* disable "new GPIO\Pins()" calls */
  pinsObjectHandlers.get_constructor = getConstructorObjectHandler;
  /* disable object cloning */
  pinsObjectHandlers.clone_obj = NULL;
  /* Handler for objects that cannot be meaningfully compared. */
  /* pinsObjectHandlers.compare = zend_objects_not_comparable; */
  /* sets the memory offset between pinsObject and zendObject inside struct _pinsObject */
  pinsObjectHandlers.offset = XtOffsetOf(pinsObject, zendObject);
  /* hook the object release */
  pinsObjectHandlers.free_obj = pinsFreeObject;
  /* hook the object property unset */
  pinsObjectHandlers.unset_property = unsetPropertyObjectHandler;

  /* GPIO\Pins implements \Countable, \ArrayAccess and \Iterator */
  zend_class_implements(classEntry, 3, zend_ce_countable, zend_ce_arrayaccess, zend_ce_iterator);

  zval propDefaultValue;
  zend_string *propName;
  /* default property value (undefined) */
  ZVAL_UNDEF(&propDefaultValue);

  /* private GPIO\Chip $chip */
  propName = zend_string_init("chip", sizeof("chip") - 1, false);
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_OBJECT)
  );
  zend_string_release(propName);

  return classEntry;
}

zend_object *pinsCreateObject(zend_class_entry *zceClass) {
  pinsObject *self = zend_object_alloc(sizeof(pinsObject), zceClass);
  self->v2 = 0;
  self->cfd = -1;
  self->offsets = NULL;
  self->count = 0;
  self->curr = 0;

  zend_object_std_init(&self->zendObject, zceClass);
  object_properties_init(&self->zendObject, zceClass);

  /* set object handlers */
  self->zendObject.handlers = &pinsObjectHandlers;

  return &self->zendObject;
}

void pinsSetData(zend_object *obj, const int v2, const int cfd, unsigned int *offsets, const unsigned int count) {
  pinsObject *self = getPinsObject(obj);
  self->v2 = v2;
  self->cfd = cfd;
  self->offsets = offsets;
  self->count = count;
  self->curr = 0;
}

/* {{{ GPIO\Pins::getChip(): GPIO\Chip */
PHP_METHOD(GPIO_Pins, getChip) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *chip = zend_read_property(zcePins, Z_OBJ_P(ZEND_THIS), "chip", sizeof("chip") - 1, true, &rv);

  RETURN_OBJ(Z_OBJ_P(chip));
}
/* }}} */

/* {{{ GPIO\Pins::count(): int */
PHP_METHOD(GPIO_Pins, count) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinsObject *self = getPinsObject(Z_OBJ_P(ZEND_THIS));

  RETURN_LONG(self->count);
}
/* }}} */

/* {{{ GPIO\Pins::offsetExists(mixed $offset): bool */
PHP_METHOD(GPIO_Pins, offsetExists) {
  zend_long offset;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(offset)
  ZEND_PARSE_PARAMETERS_END();

  pinsObject *self = getPinsObject(Z_OBJ_P(ZEND_THIS));

  RETURN_BOOL(offset >= 0 && offset < self->count);
}
/* }}} */

/* {{{ GPIO\Pins::offsetGet(mixed $offset): mixed */
PHP_METHOD(GPIO_Pins, offsetGet) {
  zend_long offset;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(offset)
  ZEND_PARSE_PARAMETERS_END();

  pinsObject *self = getPinsObject(Z_OBJ_P(ZEND_THIS));

  if (offset < 0) {
    zend_throw_exception_ex(zceException, EINVAL, "Invalid offset, cannot be negative");

    RETURN_THROWS();
  }

  if (offset >= self->count) {
    zend_throw_exception_ex(zceException, EINVAL, "Invalid offset, cannot be greater than the number of pins");

    RETURN_THROWS();
  }

  /* create a new GPIO\Pin instance */
  zend_object *pin = pinCreateObject(zcePin);
  pinSetData(pin, self->v2, self->cfd, self->offsets[offset]);

  zval rv;
  zval *chip = zend_read_property(zcePins, Z_OBJ_P(ZEND_THIS), "chip", sizeof("chip") - 1, true, &rv);

  /* update GPIO\Pin::$chip instance property with reference to $this->chip */
  zend_update_property(zcePin, pin, "chip", sizeof("chip") - 1, chip);

  RETURN_OBJ(pin);
}
/* }}} */

/* {{{ GPIO\Pins::offsetSet(mixed $offset, mixed $value): void */
PHP_METHOD(GPIO_Pins, offsetSet) {
  zend_long offset, value;
  ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(offset)
    Z_PARAM_LONG(value)
  ZEND_PARSE_PARAMETERS_END();

  zend_throw_error(NULL, "Cannot set GPIO\\Pins pin at offset %d", offset);

  RETURN_THROWS();
}
/* }}} */

/* {{{ GPIO\Pins::offsetUnset(mixed $offset): void */
PHP_METHOD(GPIO_Pins, offsetUnset) {
  zend_long offset;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(offset)
  ZEND_PARSE_PARAMETERS_END();

  zend_throw_error(NULL, "Cannot unset GPIO\\Pins pin at offset %d", offset);

  RETURN_THROWS();
}
/* }}} */

/* {{{ GPIO\Pins::current(): mixed */
PHP_METHOD(GPIO_Pins, current) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinsObject *self = getPinsObject(Z_OBJ_P(ZEND_THIS));
  if (self->curr >= self->count) {
    zend_throw_exception_ex(zceException, EINVAL, "Failed to get current pin: %s", strerror(EINVAL));

    RETURN_THROWS();
  }

  /* create a new GPIO\Pin instance */
  zend_object *pin = pinCreateObject(zcePin);
  pinSetData(pin, self->v2, self->cfd, self->offsets[self->curr]);

  zval rv;
  zval *chip = zend_read_property(zcePins, Z_OBJ_P(ZEND_THIS), "chip", sizeof("chip") - 1, true, &rv);

  /* update GPIO\Pin::$chip instance property with reference to $this->chip */
  zend_update_property(zcePin, pin, "chip", sizeof("chip") - 1, chip);

  RETURN_OBJ(pin);
}
/* }}} */

/* {{{ GPIO\Pins::key(): mixed */
PHP_METHOD(GPIO_Pins, key) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinsObject *self = getPinsObject(Z_OBJ_P(ZEND_THIS));

  RETURN_LONG(self->curr);
}
/* }}} */

/* {{{ GPIO\Pins::next(): void */
PHP_METHOD(GPIO_Pins, next) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinsObject *self = getPinsObject(Z_OBJ_P(ZEND_THIS));
  self->curr++;
}
/* }}} */

/* {{{ GPIO\Pins::rewind(): void */
PHP_METHOD(GPIO_Pins, rewind) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinsObject *self = getPinsObject(Z_OBJ_P(ZEND_THIS));
  self->curr = 0;
}
/* }}} */

/* {{{ GPIO\Pins::valid(): bool */
PHP_METHOD(GPIO_Pins, valid) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinsObject *self = getPinsObject(Z_OBJ_P(ZEND_THIS));

  RETURN_BOOL(self->curr >= 0 && self->curr < self->count);
}
/* }}} */
