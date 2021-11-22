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

#include "exception.h"
#include "gpio_arginfo.h"
#include "zend_exceptions.h"

zend_class_entry* registerExceptionClass(void) {
  zend_class_entry ce, *classEntry;

  INIT_CLASS_ENTRY(ce, "GPIO\\Exception", class_GPIO_Exception_methods);
  /* GPIO\Exception extends \Exception (zend_ce_exception) */
  classEntry = zend_register_internal_class_ex(&ce, zend_ce_exception);

  return classEntry;
}
