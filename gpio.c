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
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "gpio.h"
#include "gpio_arginfo.h"

#include "src/chip.h"
#include "src/event.h"
#include "src/exception.h"
#include "src/pin.h"
#include "src/pins.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
  ZEND_PARSE_PARAMETERS_START(0, 0) \
  ZEND_PARSE_PARAMETERS_END()
#endif

/* Class entry pointers (global resources) */
zend_class_entry *zceChip;
zend_class_entry *zceException;
zend_class_entry *zceEvent;
zend_class_entry *zcePin;
zend_class_entry *zcePins;

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(gpio) {
  #if defined(ZTS) && defined(COMPILE_DL_GPIO)
    ZEND_TSRMLS_CACHE_UPDATE();
  #endif

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(gpio) {
  /* Extension constants */
  REGISTER_STRING_CONSTANT("GPIO\\EXTENSION_VERSION", PHP_GPIO_VERSION, CONST_CS | CONST_PERSISTENT);

  /* Class Registration (from each *.h file) */
  zceChip = registerChipClass();
  zceEvent = registerEventClass();
  zceException = registerExceptionClass();
  zcePin = registerPinClass();
  zcePins = registerPinsClass();

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(gpio) {
  php_info_print_table_start();
  php_info_print_table_header(2, "gpio support", "enabled");
  php_info_print_table_row(2, "Extension version", PHP_GPIO_VERSION);
  php_info_print_table_end();
}
/* }}} */

/* {{{ gpio_module_entry */
zend_module_entry gpio_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_GPIO_EXTNAME, /* Extension name */
  NULL,             /* zend_function_entry */
  PHP_MINIT(gpio),  /* PHP_MINIT     - Module initialization */
  NULL,             /* PHP_MSHUTDOWN - Module shutdown */
  PHP_RINIT(gpio),  /* PHP_RINIT     - Request initialization */
  NULL,             /* PHP_RSHUTDOWN - Request shutdown */
  PHP_MINFO(gpio),  /* PHP_MINFO     - Module info */
  PHP_GPIO_VERSION, /* Version */
  STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GPIO
  #ifdef ZTS
    ZEND_TSRMLS_CACHE_DEFINE()
  #endif
  ZEND_GET_MODULE(gpio)
#endif
