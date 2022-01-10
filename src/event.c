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
#include "event.h"
#include "pin.h"
#include "gpio.h"
#include "gpio_arginfo.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_object_handlers.h"

// #include <errno.h>
// #include <fcntl.h>
// #include <limits.h>
#include <linux/gpio.h>
// #include <poll.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/ioctl.h>
// #include <sys/stat.h>
// #include <sys/sysmacros.h>
// #include <sys/types.h>
// #include <unistd.h>

static zend_object_handlers eventObjectHandlers;

/********************************/
/* Extension's Internal Methods */
/********************************/

/* custom __construct handler */
static zend_function *getConstructorObjectHandler(zend_object *obj) {
  zend_throw_error(NULL, "Cannot directly construct GPIO\\Event");

  return NULL;
}

/* custom unset($inst->prop) handler */
static void unsetPropertyObjectHandler(zend_object *object, zend_string *offset, void **cache_slot) {
  zend_throw_error(NULL, "Cannot unset GPIO\\Event::$%s property", ZSTR_VAL(offset));
}

/********************************/
/* Extension's External Methods */
/********************************/

zend_class_entry* registerEventClass(void) {
  zend_class_entry ce, *classEntry;

  INIT_CLASS_ENTRY(ce, "GPIO\\Event", class_GPIO_Event_methods);
  classEntry = zend_register_internal_class(&ce);
  /* Final class / Objects of this class may not have dynamic properties */
  classEntry->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
  /* intercept object creation to change object handlers */
  classEntry->create_object = eventCreateObject;

  /* disable serialization/unserialization */
  #ifdef ZEND_ACC_NOT_SERIALIZABLE
    classEntry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
  #else
    classEntry->serialize = zend_class_serialize_deny;
    classEntry->unserialize = zend_class_unserialize_deny;
  #endif

  /* Class Constants */
  zend_declare_class_constant_long(classEntry, "RISING_EDGE", sizeof("RISING_EDGE") - 1, RISING_EDGE);
  zend_declare_class_constant_long(classEntry, "FALLING_EDGE", sizeof("FALLING_EDGE") - 1, FALLING_EDGE);

  /* initialize eventObjectHandlers with standard object handlers */
  memcpy(&eventObjectHandlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

  /* disable "new GPIO\Event()" calls */
  eventObjectHandlers.get_constructor = getConstructorObjectHandler;
  /* disable object cloning */
  eventObjectHandlers.clone_obj = NULL;
  /* Handler for objects that cannot be meaningfully compared. */
  /* eventObjectHandlers.compare = zend_objects_not_comparable; */
  /* hook the object property unset */
  eventObjectHandlers.unset_property = unsetPropertyObjectHandler;

  zval propDefaultValue;
  zend_string *propName;
  /* default property value (undefined) */
  ZVAL_UNDEF(&propDefaultValue);

  /* private int $eventType */
  propName = zend_string_init("eventType", sizeof("eventType") - 1, false);
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_LONG)
  );
  zend_string_release(propName);

  /* private int $timestamp */
  propName = zend_string_init("timestamp", sizeof("timestamp") - 1, false);
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

zend_object *eventCreateObject(zend_class_entry *zceClass) {
  zend_object *zendObject = zend_object_alloc(sizeof(zend_object), zceClass);

  zend_object_std_init(zendObject, zceClass);
  object_properties_init(zendObject, zceClass);

  /* set object handlers */
  zendObject->handlers = &eventObjectHandlers;

  return zendObject;
}

/********************************/
/* PHP Visible Methods          */
/********************************/

/* {{{ GPIO\Event::getType(): int */
PHP_METHOD(GPIO_Event, getType) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *eventType = zend_read_property(zceEvent, Z_OBJ_P(ZEND_THIS), "eventType", sizeof("eventType") - 1, true, &rv);

  RETURN_LONG(zval_get_long(eventType));
}
/* }}} */

/* {{{ GPIO\Event::getTimestamp(): int */
PHP_METHOD(GPIO_Event, getTimestamp) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *timestamp = zend_read_property(zceEvent, Z_OBJ_P(ZEND_THIS), "timestamp", sizeof("timestamp") - 1, true, &rv);

  RETURN_LONG(zval_get_long(timestamp));
}
/* }}} */
