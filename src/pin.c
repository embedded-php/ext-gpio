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
#include "pin.h"
#include "chip.h"
#include "event.h"
#include "gpio.h"
#include "gpio_arginfo.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_object_handlers.h"

#include <errno.h>
#include <fcntl.h>
// #include <limits.h>
#include <linux/gpio.h>
#include <poll.h>
// #include <stdint.h>
// #include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
// #include <sys/stat.h>
// #include <sys/sysmacros.h>
// #include <sys/types.h>
// #include <unistd.h>

/* zend_object wrapper to ensure GPIO Pin is handled properly */
struct _pinObject {
  int v2; /* ABI v2 flag */
  int cfd; /* chip file descriptor */
  int pfd; /* pin file descriptor */
  unsigned int offset;
  struct gpioline_info v1info;
  struct gpiohandle_request v1req;
  struct gpioevent_request v1event;
  struct gpio_v2_line_info v2info;
  struct gpio_v2_line_request v2req;
  zend_object zendObject;
};

static zend_object_handlers pinObjectHandlers;

/********************************/
/* Extension's Internal Methods */
/********************************/

/* retrieve a pinObject pointer from a zend_object pointer */
static inline pinObject *getPinObject(zend_object *obj) {
  return (pinObject *)((char *)(obj) - XtOffsetOf(pinObject, zendObject));
}

/* handle pinObject release */
static void pinFreeObject(zend_object *obj) {
  pinObject *self = getPinObject(obj);

  /* failure to retrieve pin instance (edge case?) */
  if (self == NULL) {
    return;
  }

  /* if the pin was open (requested), close it */
  if (self->pfd >= 0) {
    close(self->pfd);
    self->pfd = -1;
  }

  /* release self->zendObject */
  zend_object_std_dtor(&self->zendObject);
}

/* custom __construct handler */
static zend_function *getConstructorObjectHandler(zend_object *obj) {
  zend_throw_error(NULL, "Cannot directly construct GPIO\\Pin");

  return NULL;
}

/* custom unset($inst->prop) handler */
static void unsetPropertyObjectHandler(zend_object *object, zend_string *offset, void **cache_slot) {
  zend_throw_error(NULL, "Cannot unset GPIO\\Pin::$%s property", ZSTR_VAL(offset));
}

/* reloads pin data from chip */
static int reloadPinData(pinObject *pin) {
  if (pin->v2) {
    /* ABI v2 */
    memset(&pin->v2info, 0, sizeof(pin->v2info));
    pin->v2info.offset = pin->offset;

    if (ioctl(pin->cfd, GPIO_V2_GET_LINEINFO_IOCTL, &pin->v2info) < 0) {
      return -1;
    }

    if (pin->v2info.name[0] == 0) {
      zend_update_property_string(zcePin, &pin->zendObject, "name", sizeof("name") - 1, "");
    } else {
      zend_update_property_string(zcePin, &pin->zendObject, "name", sizeof("name") - 1, pin->v2info.name);
    }

    if (pin->v2info.consumer[0] == 0) {
      zend_update_property_string(zcePin, &pin->zendObject, "consumer", sizeof("consumer") - 1, "");
    } else {
      zend_update_property_string(zcePin, &pin->zendObject, "consumer", sizeof("consumer") - 1, pin->v2info.consumer);
    }

    return 0;
  }

  /* ABI v1 */
  memset(&pin->v1info, 0, sizeof(pin->v1info));
  pin->v1info.line_offset = pin->offset;

  if (ioctl(pin->cfd, GPIO_GET_LINEINFO_IOCTL, &pin->v1info) < 0) {
    return -1;
  }

  if (pin->v1info.name[0] == 0) {
    zend_update_property_string(zcePin, &pin->zendObject, "name", sizeof("name") - 1, "");
  } else {
    zend_update_property_string(zcePin, &pin->zendObject, "name", sizeof("name") - 1, pin->v1info.name);
  }

  if (pin->v1info.consumer[0] == 0) {
    zend_update_property_string(zcePin, &pin->zendObject, "consumer", sizeof("consumer") - 1, "");
  } else {
    zend_update_property_string(zcePin, &pin->zendObject, "consumer", sizeof("consumer") - 1, pin->v1info.consumer);
  }

  return 0;
}

/* sets the file descriptor as non-blocking */
/* reference: https://stackoverflow.com/questions/3360797/non-blocking-socket-with-poll */
static int setNonBlocking(int fd) {
  int flags;

  flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return -1;
  }

  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) < 0) {
    return -1;
  }

  return 0;
}

/********************************/
/* Extension's External Methods */
/********************************/

zend_class_entry* registerPinClass(void) {
  zend_class_entry ce, *classEntry;

  INIT_CLASS_ENTRY(ce, "GPIO\\Pin", class_GPIO_Pin_methods);
  classEntry = zend_register_internal_class(&ce);
  /* Final class / Objects of this class may not have dynamic properties */
  classEntry->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
  /* intercept object creation to change object handlers */
  classEntry->create_object = pinCreateObject;

  /* disable serialization/unserialization */
  #ifdef ZEND_ACC_NOT_SERIALIZABLE
    classEntry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
  #else
    classEntry->serialize = zend_class_serialize_deny;
    classEntry->unserialize = zend_class_unserialize_deny;
  #endif

  /* Class Constants */
  zend_declare_class_constant_long(classEntry, "BIAS_DISABLED", sizeof("BIAS_DISABLED") - 1, BIAS_DISABLED);
  zend_declare_class_constant_long(classEntry, "BIAS_PULL_UP", sizeof("BIAS_PULL_UP") - 1, BIAS_PULL_UP);
  zend_declare_class_constant_long(classEntry, "BIAS_PULL_DOWN", sizeof("BIAS_PULL_DOWN") - 1, BIAS_PULL_DOWN);
  zend_declare_class_constant_long(classEntry, "BIAS_AS_IS", sizeof("BIAS_AS_IS") - 1, BIAS_AS_IS);
  zend_declare_class_constant_long(classEntry, "DRIVE_PUSH_PULL", sizeof("DRIVE_PUSH_PULL") - 1, DRIVE_PUSH_PULL);
  zend_declare_class_constant_long(classEntry, "DRIVE_OPEN_DRAIN", sizeof("DRIVE_OPEN_DRAIN") - 1, DRIVE_OPEN_DRAIN);
  zend_declare_class_constant_long(classEntry, "DRIVE_OPEN_SOURCE", sizeof("DRIVE_OPEN_SOURCE") - 1, DRIVE_OPEN_SOURCE);
  zend_declare_class_constant_long(classEntry, "FLAG_NONE", sizeof("FLAG_NONE") - 1, FLAG_NONE);
  zend_declare_class_constant_long(classEntry, "FLAG_OPEN_DRAIN", sizeof("FLAG_OPEN_DRAIN") - 1, FLAG_OPEN_DRAIN);
  zend_declare_class_constant_long(classEntry, "FLAG_OPEN_SOURCE", sizeof("FLAG_OPEN_SOURCE") - 1, FLAG_OPEN_SOURCE);
  zend_declare_class_constant_long(classEntry, "FLAG_ACTIVE_LOW", sizeof("FLAG_ACTIVE_LOW") - 1, FLAG_ACTIVE_LOW);
  zend_declare_class_constant_long(classEntry, "FLAG_BIAS_DISABLED", sizeof("FLAG_BIAS_DISABLED") - 1, FLAG_BIAS_DISABLED);
  zend_declare_class_constant_long(classEntry, "FLAG_BIAS_PULL_DOWN", sizeof("FLAG_BIAS_PULL_DOWN") - 1, FLAG_BIAS_PULL_DOWN);
  zend_declare_class_constant_long(classEntry, "FLAG_BIAS_PULL_UP", sizeof("FLAG_BIAS_PULL_UP") - 1, FLAG_BIAS_PULL_UP);
  zend_declare_class_constant_long(classEntry, "FALLING_EDGE", sizeof("FALLING_EDGE") - 1, FALLING_EDGE);
  zend_declare_class_constant_long(classEntry, "RISING_EDGE", sizeof("RISING_EDGE") - 1, RISING_EDGE);
  zend_declare_class_constant_long(classEntry, "BOTH_EDGES", sizeof("BOTH_EDGES") - 1, BOTH_EDGES);

  /* initialize pinObjectHandlers with standard object handlers */
  memcpy(&pinObjectHandlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

  /* disable "new GPIO\Pin()" calls */
  pinObjectHandlers.get_constructor = getConstructorObjectHandler;
  /* disable object cloning */
  pinObjectHandlers.clone_obj = NULL;
  /* Handler for objects that cannot be meaningfully compared. */
  /* pinObjectHandlers.compare = zend_objects_not_comparable; */
  /* sets the memory offset between pinObject and zendObject inside struct _pinObject */
  pinObjectHandlers.offset = XtOffsetOf(pinObject, zendObject);
  /* hook the object release */
  pinObjectHandlers.free_obj = pinFreeObject;
  /* hook the object property unset */
  pinObjectHandlers.unset_property = unsetPropertyObjectHandler;

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

  /* private string $consumer */
  propName = zend_string_init("consumer", sizeof("consumer") - 1, false);
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_STRING)
  );
  zend_string_release(propName);

  /* private int $offset */
  propName = zend_string_init("offset", sizeof("offset") - 1, false);
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

zend_object *pinCreateObject(zend_class_entry *zceClass) {
  pinObject *self = zend_object_alloc(sizeof(pinObject), zceClass);
  self->v2 = 0;
  self->cfd = -1;
  self->pfd = -1;
  self->offset = 0;

  /* ABI v2 Request Init/Defaults */
  memset(&self->v2info, 0, sizeof(self->v2info));
  memset(&self->v2req, 0, sizeof(self->v2req));
  self->v2req.fd = -1;
  self->v2req.num_lines = 1;
  self->v2req.event_buffer_size = 16;

  /* ABI v1 Request Init/Defaults */
  memset(&self->v1info, 0, sizeof(self->v1info));
  memset(&self->v1req, 0, sizeof(self->v1req));
  memset(&self->v1event, 0, sizeof(self->v1event));
  self->v1req.fd = -1;
  self->v1req.lines = 1;

  zend_object_std_init(&self->zendObject, zceClass);
  object_properties_init(&self->zendObject, zceClass);

  /* set object handlers */
  self->zendObject.handlers = &pinObjectHandlers;

  return &self->zendObject;
}

void pinSetData(zend_object *obj, const int v2, const int cfd, const unsigned int offset) {
  pinObject *self = getPinObject(obj);
  self->v2 = v2;
  self->cfd = cfd;
  self->offset = offset;

  /* ABI v2 Request */
  self->v2req.offsets[0] = offset;
  /* ABI v1 Request/Event */
  self->v1req.lineoffsets[0] = offset;
  self->v1event.lineoffset = offset;

  if (reloadPinData(self) < 0) {
    zend_throw_error(NULL, "Failed to get pin information");

    return;
  }

  /* update class properties with pin information */
  zend_update_property_long(zcePin, obj, "offset", sizeof("offset") - 1, offset);
}

/********************************/
/* PHP Visible Methods          */
/********************************/

/* {{{ GPIO\Pin::getBias(): int */
PHP_METHOD(GPIO_Pin, getBias) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));

  if (self->v2) {
    /* ABI v2 */
    if (self->v2info.flags & GPIO_V2_LINE_FLAG_BIAS_DISABLED) {
      RETURN_LONG(BIAS_DISABLED);
    }

    if (self->v2info.flags & GPIO_V2_LINE_FLAG_BIAS_PULL_UP) {
      RETURN_LONG(BIAS_PULL_UP);
    }

    if (self->v2info.flags & GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN) {
      RETURN_LONG(BIAS_PULL_DOWN);
    }

    RETURN_LONG(BIAS_AS_IS);
  }

  /* ABI v1 */
  if (self->v1info.flags & GPIOLINE_FLAG_BIAS_DISABLE) {
    RETURN_LONG(BIAS_DISABLED);
  }

  if (self->v1info.flags & GPIOLINE_FLAG_BIAS_PULL_UP) {
    RETURN_LONG(BIAS_PULL_UP);
  }

  if (self->v1info.flags & GPIOLINE_FLAG_BIAS_PULL_DOWN) {
    RETURN_LONG(BIAS_PULL_DOWN);
  }

  RETURN_LONG(BIAS_AS_IS);
}
/* }}} */

/* {{{ GPIO\Pin::getConsumer(): string */
PHP_METHOD(GPIO_Pin, getConsumer) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *consumer = zend_read_property(zcePin, Z_OBJ_P(ZEND_THIS), "consumer", sizeof("consumer") - 1, true, &rv);

  RETURN_STR(zval_get_string(consumer));
}
/* }}} */

/* {{{ GPIO\Pin::getDrive(): int */
PHP_METHOD(GPIO_Pin, getDrive) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));

  if (self->v2) {
    /* ABI v2 */
    if (self->v2info.flags & GPIO_V2_LINE_FLAG_OPEN_DRAIN) {
      RETURN_LONG(DRIVE_OPEN_DRAIN);
    }

    if (self->v2info.flags & GPIO_V2_LINE_FLAG_OPEN_SOURCE) {
      RETURN_LONG(DRIVE_OPEN_SOURCE);
    }

    RETURN_LONG(DRIVE_PUSH_PULL);
  }

  /* ABI v1 */
  if (self->v1info.flags & GPIOLINE_FLAG_OPEN_DRAIN) {
    RETURN_LONG(DRIVE_OPEN_DRAIN);
  }

  if (self->v1info.flags & GPIOLINE_FLAG_OPEN_SOURCE) {
    RETURN_LONG(DRIVE_OPEN_SOURCE);
  }

  RETURN_LONG(DRIVE_PUSH_PULL);
}
/* }}} */

/* {{{ GPIO\Pin::getChip(): Chip */
PHP_METHOD(GPIO_Pin, getChip) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *chip = zend_read_property(zcePin, Z_OBJ_P(ZEND_THIS), "chip", sizeof("chip") - 1, true, &rv);

  RETURN_OBJ(Z_OBJ_P(chip));
}
/* }}} */

/* {{{ GPIO\Pin::isActiveLow(): bool */
PHP_METHOD(GPIO_Pin, isActiveLow) {
ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));

  if (self->v2) {
    /* ABI v2 */
    RETURN_BOOL(self->v2info.flags & GPIO_V2_LINE_FLAG_ACTIVE_LOW);
  }

  /* ABI v1 */
  RETURN_BOOL(self->v1info.flags & GPIOLINE_FLAG_ACTIVE_LOW);
}
/* }}} */

/* {{{ GPIO\Pin::isUsed(): bool */
PHP_METHOD(GPIO_Pin, isUsed) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));

  if (self->v2) {
    /* ABI v2 */
    RETURN_BOOL(self->v2info.flags & GPIO_V2_LINE_FLAG_USED);
  }

  /* ABI v1 */
  RETURN_BOOL(self->v1info.flags & GPIOLINE_FLAG_KERNEL);
}
/* }}} */

/* {{{ GPIO\Pin::getName(): string */
PHP_METHOD(GPIO_Pin, getName) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *name = zend_read_property(zcePin, Z_OBJ_P(ZEND_THIS), "name", sizeof("name") - 1, true, &rv);

  RETURN_STR(zval_get_string(name));
}
/* }}} */

/* {{{ GPIO\Pin::getOffset(): int */
PHP_METHOD(GPIO_Pin, getOffset) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *offset = zend_read_property(zcePin, Z_OBJ_P(ZEND_THIS), "offset", sizeof("offset") - 1, true, &rv);

  RETURN_LONG(zval_get_long(offset));
}
/* }}} */

/* {{{ GPIO\Pin::asInput(string $consumer, int $flags = self::FLAG_NONE, int $debouncePeriod = 0): self */
PHP_METHOD(GPIO_Pin, asInput) {
  char *consumer;
  size_t consumerLen;
  zend_long flags = FLAG_NONE;
  zend_long debouncePeriod = 0;
  ZEND_PARSE_PARAMETERS_START(1, 3)
    Z_PARAM_STRING(consumer, consumerLen)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(flags)
    Z_PARAM_LONG(debouncePeriod)
  ZEND_PARSE_PARAMETERS_END();

  int bias = 0;
  if (flags & FLAG_BIAS_DISABLED) {
    bias++;
  }

  if (flags & FLAG_BIAS_PULL_DOWN) {
    bias++;
  }

  if (flags & FLAG_BIAS_PULL_UP) {
    bias++;
  }

  if (bias > 1) {
    zend_throw_exception_ex(zceException, EINVAL, "Only a single FLAG_BIAS_* flag can be set at a time");

    RETURN_THROWS();
  }

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));

  if (self->v2) {
    /* ABI v2 */
    strncpy(self->v2req.consumer, consumer, sizeof(self->v2req.consumer));
    if (debouncePeriod > 0) {
      self->v2req.config.num_attrs = 1;
      self->v2req.config.attrs[0].mask = 1ULL;
      self->v2req.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
      self->v2req.config.attrs[0].attr.debounce_period_us = debouncePeriod;
    }

    /* reset all flags */
    self->v2req.config.flags = 0;
    self->v2req.config.flags |= GPIO_V2_LINE_FLAG_INPUT;

    if (flags & FLAG_ACTIVE_LOW) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_ACTIVE_LOW;
    }

    if (flags & FLAG_BIAS_DISABLED) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_BIAS_DISABLED;
    }

    if (flags & FLAG_BIAS_PULL_DOWN) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
    }

    if (flags & FLAG_BIAS_PULL_UP) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_UP;
    }

    if (self->pfd >= 0) {
      /* pin has been requested previously, just update its flags instead */
      if (ioctl(self->pfd, GPIO_V2_LINE_SET_CONFIG_IOCTL, &self->v2req.config) < 0) {
        zend_throw_exception_ex(zceException, errno, "Failed to update pin flags: %s", strerror(errno));

        RETURN_THROWS();
      }

      RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
    }

    if (ioctl(self->cfd, GPIO_V2_GET_LINE_IOCTL, &self->v2req) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to request pin as input: %s", strerror(errno));

      RETURN_THROWS();
    }

    if (setNonBlocking(self->v2req.fd) < 0) {
      close(self->v2req.fd);
      zend_throw_exception_ex(zceException, errno, "Failed to setup pin: %s", strerror(errno));

      RETURN_THROWS();
    }

    self->pfd = self->v2req.fd;

    if (reloadPinData(self) < 0) {
      zend_throw_error(NULL, "Failed to get pin information");

      return;
    }

    RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
  }

  /* ABI v1 */
  strncpy(self->v1req.consumer_label, consumer, sizeof(self->v1req.consumer_label));

  /* reset all flags */
  self->v1req.flags = 0;
  self->v1req.flags |= GPIOHANDLE_REQUEST_INPUT;
  if (flags & FLAG_ACTIVE_LOW) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_ACTIVE_LOW;
  }

  if (flags & FLAG_BIAS_DISABLED) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_BIAS_DISABLE;
  }

  if (flags & FLAG_BIAS_PULL_DOWN) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_BIAS_PULL_DOWN;
  }

  if (flags & FLAG_BIAS_PULL_UP) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_BIAS_PULL_UP;
  }

  if (self->pfd >= 0) {
    /* pin has been requested previously, just update its flags instead */
    struct gpiohandle_config config;
    memset(&config, 0, sizeof(config));
    config.flags = self->v1req.flags;
    memcpy(config.default_values, self->v1req.default_values, sizeof(config.default_values));
    if (ioctl(self->pfd, GPIOHANDLE_SET_CONFIG_IOCTL, &config) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to update pin flags: %s", strerror(errno));

      RETURN_THROWS();
    }

    RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
  }

  if (ioctl(self->cfd, GPIO_GET_LINEHANDLE_IOCTL, &self->v1req) < 0) {
    zend_throw_exception_ex(zceException, errno, "Failed to request pin as input: %s", strerror(errno));

    RETURN_THROWS();
  }

  if (setNonBlocking(self->v1req.fd) < 0) {
    close(self->v1req.fd);
    zend_throw_exception_ex(zceException, errno, "Failed to setup pin: %s", strerror(errno));

    RETURN_THROWS();
  }

  self->pfd = self->v1req.fd;

  if (reloadPinData(self) < 0) {
    zend_throw_error(NULL, "Failed to get pin information");

    return;
  }

  RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}
/* }}} */

/* {{{ GPIO\Pin::asOutput(string $consumer, int $flags = self::FLAG_NONE, bool $setHigh = false): self */
PHP_METHOD(GPIO_Pin, asOutput) {
  char *consumer;
  size_t consumerLen;
  zend_long flags = FLAG_NONE;
  bool setHigh = 0;
  ZEND_PARSE_PARAMETERS_START(1, 3)
    Z_PARAM_STRING(consumer, consumerLen)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(flags)
    Z_PARAM_BOOL(setHigh)
  ZEND_PARSE_PARAMETERS_END();

  if (flags & FLAG_OPEN_DRAIN && flags & FLAG_OPEN_SOURCE) {
    zend_throw_exception_ex(zceException, EINVAL, "Only a single FLAG_OPEN_* flag can be set at a time");

    RETURN_THROWS();
  }

  int bias = 0;
  if (flags & FLAG_BIAS_DISABLED) {
    bias++;
  }

  if (flags & FLAG_BIAS_PULL_DOWN) {
    bias++;
  }

  if (flags & FLAG_BIAS_PULL_UP) {
    bias++;
  }

  if (bias > 1) {
    zend_throw_exception_ex(zceException, EINVAL, "Only a single FLAG_BIAS_* flag can be set at a time");

    RETURN_THROWS();
  }

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));

  if (self->v2) {
    /* ABI v2 */
    strncpy(self->v2req.consumer, consumer, sizeof(self->v2req.consumer));
    if (setHigh) {
      self->v2req.config.num_attrs = 1;
      self->v2req.config.attrs[0].mask = 1ULL;
      self->v2req.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
      self->v2req.config.attrs[0].attr.values = 1ULL;
    }

    /* reset all flags */
    self->v2req.config.flags = 0;
    self->v2req.config.flags |= GPIO_V2_LINE_FLAG_OUTPUT;
    if (flags & FLAG_OPEN_DRAIN) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_OPEN_DRAIN;
    }

    if (flags & FLAG_OPEN_SOURCE) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_OPEN_SOURCE;
    }

    if (flags & FLAG_ACTIVE_LOW) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_ACTIVE_LOW;
    }

    if (flags & FLAG_BIAS_DISABLED) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_BIAS_DISABLED;
    }

    if (flags & FLAG_BIAS_PULL_DOWN) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
    }

    if (flags & FLAG_BIAS_PULL_UP) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_UP;
    }

    if (self->pfd >= 0) {
      /* pin has been requested previously, just update its flags instead */
      if (ioctl(self->pfd, GPIO_V2_LINE_SET_CONFIG_IOCTL, &self->v2req.config) < 0) {
        zend_throw_exception_ex(zceException, errno, "Failed to update pin flags: %s", strerror(errno));

        RETURN_THROWS();
      }

      RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
    }

    if (ioctl(self->cfd, GPIO_V2_GET_LINE_IOCTL, &self->v2req) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to request pin as output: %s", strerror(errno));

      RETURN_THROWS();
    }

    if (setNonBlocking(self->v2req.fd) < 0) {
      close(self->v2req.fd);
      zend_throw_exception_ex(zceException, errno, "Failed to setup pin: %s", strerror(errno));

      RETURN_THROWS();
    }

    self->pfd = self->v2req.fd;

    if (reloadPinData(self) < 0) {
      zend_throw_error(NULL, "Failed to get pin information");

      return;
    }

    RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
  }

  /* ABI v1 */
  strncpy(self->v1req.consumer_label, consumer, sizeof(self->v1req.consumer_label));
  if (setHigh) {
    self->v1req.default_values[0] = 1;
  }

  /* reset all flags */
  self->v1req.flags = 0;
  self->v1req.flags |= GPIOHANDLE_REQUEST_OUTPUT;
  if (flags & FLAG_OPEN_DRAIN) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_OPEN_DRAIN;
  }

  if (flags & FLAG_OPEN_SOURCE) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_OPEN_SOURCE;
  }

  if (flags & FLAG_ACTIVE_LOW) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_ACTIVE_LOW;
  }

  if (flags & FLAG_BIAS_DISABLED) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_BIAS_DISABLE;
  }

  if (flags & FLAG_BIAS_PULL_DOWN) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_BIAS_PULL_DOWN;
  }

  if (flags & FLAG_BIAS_PULL_UP) {
    self->v1req.flags |= GPIOHANDLE_REQUEST_BIAS_PULL_UP;
  }

  if (self->pfd >= 0) {
    /* pin has been requested previously, just update its flags instead */
    struct gpiohandle_config config;
    memset(&config, 0, sizeof(config));

    config.flags = self->v1req.flags;
    memcpy(config.default_values, self->v1req.default_values, sizeof(config.default_values));
    if (ioctl(self->pfd, GPIOHANDLE_SET_CONFIG_IOCTL, &config) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to update pin flags: %s", strerror(errno));

      RETURN_THROWS();
    }

    RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
  }

  if (ioctl(self->cfd, GPIO_GET_LINEHANDLE_IOCTL, &self->v1req) < 0) {
    zend_throw_exception_ex(zceException, errno, "Failed to request pin as output: %s", strerror(errno));

    RETURN_THROWS();
  }

  if (setNonBlocking(self->v1req.fd) < 0) {
    close(self->v1req.fd);
    zend_throw_exception_ex(zceException, errno, "Failed to setup pin: %s", strerror(errno));

    RETURN_THROWS();
  }

  self->pfd = self->v1req.fd;

  if (reloadPinData(self) < 0) {
    zend_throw_error(NULL, "Failed to get pin information");

    return;
  }

  RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}
/* }}} */

/* {{{ GPIO\Pin::isInput(): bool */
PHP_METHOD(GPIO_Pin, isInput) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));

  if (self->v2) {
    /* ABI v2 */
    RETURN_BOOL(self->v2info.flags & GPIO_V2_LINE_FLAG_INPUT);
  }

  /* ABI v1 */
  RETURN_BOOL((self->v1info.flags & GPIOLINE_FLAG_IS_OUT) != GPIOLINE_FLAG_IS_OUT);
}
/* }}} */

/* {{{ GPIO\Pin::isOutput(): bool */
PHP_METHOD(GPIO_Pin, isOutput) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));

  if (self->v2) {
    /* ABI v2 */
    RETURN_BOOL(self->v2info.flags & GPIO_V2_LINE_FLAG_OUTPUT);
  }

  /* ABI v1 */
  RETURN_BOOL(self->v1info.flags & GPIOLINE_FLAG_IS_OUT);
}
/* }}} */

/* {{{ GPIO\Pin::isHigh(): bool */
PHP_METHOD(GPIO_Pin, isHigh) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));
  if (self->pfd < 0) {
    zend_throw_exception_ex(zceException, EINVAL, "Cannot get pin value before requesting it as input or output");

    RETURN_THROWS();
  }

  if (self->v2) {
    /* ABI v2 */
    struct gpio_v2_line_values data;
    memset(&data, 0, sizeof(data));
    data.mask = 1ULL;

    if (ioctl(self->pfd, GPIO_V2_LINE_GET_VALUES_IOCTL, &data) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to get pin value: %s", strerror(errno));

      RETURN_THROWS();
    }

    RETURN_BOOL(data.bits & 1ULL);
  }

  /* ABI v1 */
  struct gpiohandle_data data;
  memset(&data, 0, sizeof(data));

  if (ioctl(self->pfd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
    zend_throw_exception_ex(zceException, errno, "Failed to get pin value: %s", strerror(errno));

    RETURN_THROWS();
  }

  RETURN_BOOL(data.values[0]);
}
/* }}} */

/* {{{ GPIO\Pin::isLow(): bool */
PHP_METHOD(GPIO_Pin, isLow) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));
  if (self->pfd < 0) {
    zend_throw_exception_ex(zceException, EINVAL, "Cannot get pin value before requesting it as input or output");

    RETURN_THROWS();
  }

  if (self->v2) {
    /* ABI v2 */
    struct gpio_v2_line_values data;
    memset(&data, 0, sizeof(data));
    data.mask = 1ULL;

    if (ioctl(self->pfd, GPIO_V2_LINE_GET_VALUES_IOCTL, &data) < 0) {
      zend_throw_exception_ex(zceException, 0, "Failed to get pin value");

      RETURN_THROWS();
    }

    RETURN_BOOL(data.bits == 0ULL);
  }

  /* ABI v1 */
  struct gpiohandle_data data;
  memset(&data, 0, sizeof(data));

  if (ioctl(self->pfd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
    zend_throw_exception_ex(zceException, 0, "Failed to get pin value");

    RETURN_THROWS();
  }

  RETURN_BOOL(data.values[0] == 0);
}
/* }}} */

/* {{{ GPIO\Pin::setHigh(): self */
PHP_METHOD(GPIO_Pin, setHigh) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));
  if (self->pfd < 0) {
    zend_throw_exception_ex(zceException, EINVAL, "Cannot set pin value before setting it as output");

    RETURN_THROWS();
  }

  if (self->v2) {
    /* ABI v2 */
    if ((self->v2info.flags & GPIO_V2_LINE_FLAG_OUTPUT) != GPIO_V2_LINE_FLAG_OUTPUT) {
      zend_throw_exception_ex(zceException, EINVAL, "Cannot call setHigh on an input pin: %s", strerror(EINVAL));

      RETURN_THROWS();
    }

    struct gpio_v2_line_values data;
    memset(&data, 0, sizeof(data));
    data.bits = 1ULL;
    data.mask = 1ULL;

    if (ioctl(self->pfd, GPIO_V2_LINE_SET_VALUES_IOCTL, &data) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to set output to high: %s", strerror(errno));

      RETURN_THROWS();
    }

    RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
  }

  /* ABI v1 */
  if ((self->v1info.flags & GPIOLINE_FLAG_IS_OUT) != GPIOLINE_FLAG_IS_OUT) {
    zend_throw_exception_ex(zceException, EINVAL, "Cannot call setHigh on an input pin: %s", strerror(EINVAL));

    RETURN_THROWS();
  }

  struct gpiohandle_data data;
  memset(&data, 0, sizeof(data));
  data.values[0] = (uint8_t)1;

  if (ioctl(self->pfd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
    zend_throw_exception_ex(zceException, errno, "Failed to set output to high: %s", strerror(errno));

    RETURN_THROWS();
  }

  RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}
/* }}} */

/* {{{ GPIO\Pin::setLow(): self */
PHP_METHOD(GPIO_Pin, setLow) {
  ZEND_PARSE_PARAMETERS_NONE();

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));
  if (self->pfd < 0) {
    zend_throw_exception_ex(zceException, EINVAL, "Cannot set pin value before setting it as output");

    RETURN_THROWS();
  }

  if (self->v2) {
    /* ABI v2 */
    if ((self->v2info.flags & GPIO_V2_LINE_FLAG_OUTPUT) != GPIO_V2_LINE_FLAG_OUTPUT) {
      zend_throw_exception_ex(zceException, EINVAL, "Cannot call setLow on an input pin: %s", strerror(EINVAL));

      RETURN_THROWS();
    }

    struct gpio_v2_line_values data;
    memset(&data, 0, sizeof(data));
    data.mask = 1ULL;

    if (ioctl(self->pfd, GPIO_V2_LINE_SET_VALUES_IOCTL, &data) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to set output to low: %s", strerror(errno));

      RETURN_THROWS();
    }

    RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
  }

  /* ABI v1 */
  if ((self->v1info.flags & GPIOLINE_FLAG_IS_OUT) != GPIOLINE_FLAG_IS_OUT) {
    zend_throw_exception_ex(zceException, EINVAL, "Cannot call setLow on an input pin: %s", strerror(EINVAL));

    RETURN_THROWS();
  }

  struct gpiohandle_data data;
  memset(&data, 0, sizeof(data));

  if (ioctl(self->pfd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
    zend_throw_exception_ex(zceException, errno, "Failed to set output to low: %s", strerror(errno));

    RETURN_THROWS();
  }

  RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}
/* }}} */

/* {{{ GPIO\Pin::waitForEdge(int $seconds, int $nanoseconds = 0, int $edgeType = self::BOTH_EDGES): bool */
PHP_METHOD(GPIO_Pin, waitForEdge) {
  zend_long seconds;
  zend_long nanoseconds = 0;
  zend_long edgeType = BOTH_EDGES;
  ZEND_PARSE_PARAMETERS_START(1, 3)
    Z_PARAM_LONG(seconds)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(nanoseconds)
    Z_PARAM_LONG(edgeType);
  ZEND_PARSE_PARAMETERS_END();

  struct timespec timeout;
  struct pollfd pfd;
  int rv;

  pinObject *self = getPinObject(Z_OBJ_P(ZEND_THIS));
  if (self->pfd < 0) {
    zend_throw_exception_ex(zceException, EINVAL, "Cannot wait for pin edge change before requesting it as input");

    RETURN_THROWS();
  }

  if (self->v2) {
    /* ABI v2 */

    /* clean previously set event flags */
    self->v2req.config.flags &= ~GPIO_V2_LINE_FLAG_EDGE_RISING;
    self->v2req.config.flags &= ~GPIO_V2_LINE_FLAG_EDGE_FALLING;

    if (edgeType & RISING_EDGE) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_EDGE_RISING;
    }

    if (edgeType & FALLING_EDGE) {
      self->v2req.config.flags |= GPIO_V2_LINE_FLAG_EDGE_FALLING;
    }

    if (ioctl(self->pfd, GPIO_V2_LINE_SET_CONFIG_IOCTL, &self->v2req.config) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to update pin flags: %s", strerror(errno));

      RETURN_THROWS();
    }
  } else {
    /* ABI v1 */
    self->v1event.handleflags = self->v1info.flags;
    strncpy(self->v1event.consumer_label, self->v1req.consumer_label, sizeof(self->v1event.consumer_label));

    /* clean previously set event flags */
    self->v1event.eventflags &= ~GPIOEVENT_REQUEST_RISING_EDGE;
    self->v1event.eventflags &= ~GPIOEVENT_REQUEST_FALLING_EDGE;

    if (edgeType & RISING_EDGE) {
      self->v1event.eventflags |= GPIOEVENT_REQUEST_RISING_EDGE;
    }

    if (edgeType & FALLING_EDGE) {
      self->v1event.eventflags |= GPIOEVENT_REQUEST_FALLING_EDGE;
    }

    /* close GPIO_GET_LINEHANDLE_IOCTL related fd (must be done before requesting GPIO_GET_LINEEVENT_IOCTL */
    close(self->pfd);
    self->pfd = -1;

    /* note that the call below is sent to the chip's fd and not the pin's fd */
    if (ioctl(self->cfd, GPIO_GET_LINEEVENT_IOCTL, &self->v1event) < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to update pin flags: %s", strerror(errno));

      RETURN_THROWS();
    }

    /* replace the now closed GPIO_GET_LINEHANDLE_IOCTL fd with GPIO_GET_LINEEVENT_IOCTL's fd */
    self->pfd = self->v1event.fd;
  }

  memset(&pfd, 0, sizeof(pfd));
  pfd.fd = self->pfd;
  pfd.events = POLLIN | POLLPRI;

  memset(&timeout, 0, sizeof(timeout));
  timeout.tv_sec = seconds;
  timeout.tv_nsec = nanoseconds;

  rv = ppoll(&pfd, 1, &timeout, NULL);
  if (rv < 0) {
    zend_throw_exception_ex(zceException, errno, "Failed wait for edge change: %s", strerror(errno));

    RETURN_THROWS();
  }

  if (rv == 0) {
    RETURN_NULL();
  }

  if (self->v2) {
    /* ABI v2 */
    struct gpio_v2_line_event data;
    memset(&data, 0, sizeof(data));
    rv = read(self->pfd, &data, sizeof(data));
    if (rv < 0) {
      zend_throw_exception_ex(zceException, errno, "Failed to read event data: %s", strerror(errno));

      RETURN_THROWS();
    }

    if ((unsigned int)rv < sizeof(data)) {
      zend_throw_exception_ex(zceException, EIO, "Failed to read event data");

      RETURN_THROWS();
    }

    zend_object *event = eventCreateObject(zceEvent);

    zend_update_property_long(zceEvent, event, "timestamp", sizeof("timestamp") - 1, data.timestamp_ns / 1000ULL);
    if (data.id & GPIO_V2_LINE_EVENT_RISING_EDGE) {
      zend_update_property_long(zceEvent, event, "eventType", sizeof("eventType") - 1, RISING_EDGE);
    }

    if (data.id & GPIO_V2_LINE_EVENT_FALLING_EDGE) {
      zend_update_property_long(zceEvent, event, "eventType", sizeof("eventType") - 1, FALLING_EDGE);
    }

    RETURN_OBJ(event);
  }

  /* ABI v1 */
  struct gpioevent_data data;
  memset(&data, 0, sizeof(data));
  rv = read(self->pfd, &data, sizeof(data));
  if (rv < 0) {
    zend_throw_exception_ex(zceException, errno, "Failed to read event data: %s", strerror(errno));

    RETURN_THROWS();
  }

  if ((unsigned int)rv < sizeof(data)) {
    zend_throw_exception_ex(zceException, EIO, "Failed to read event data");

    RETURN_THROWS();
  }

  zend_object *event = eventCreateObject(zceEvent);

  zend_update_property_long(zceEvent, event, "timestamp", sizeof("timestamp") - 1, data.timestamp / 1000ULL);
  if (data.id & GPIOEVENT_EVENT_RISING_EDGE) {
    zend_update_property_long(zceEvent, event, "eventType", sizeof("eventType") - 1, RISING_EDGE);
  }

  if (data.id & GPIOEVENT_EVENT_FALLING_EDGE) {
    zend_update_property_long(zceEvent, event, "eventType", sizeof("eventType") - 1, FALLING_EDGE);
  }

  RETURN_OBJ(event);
}
/* }}} */
