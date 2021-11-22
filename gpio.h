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

#ifndef PHP_GPIO_H
#define PHP_GPIO_H

  extern zend_module_entry gpio_module_entry;
  #define phpext_gpio_ptr &gpio_module_entry

  #define PHP_GPIO_EXTNAME   "phpgpio"
  #define PHP_GPIO_VERSION   "0.0.1-dev"
  #define PHP_GPIO_AUTHOR    "flavioheleno"
  #define PHP_GPIO_URL       "https://github.com/flavioheleno/phpgpio"
  #define PHP_GPIO_COPYRIGHT "Copyright (c) 2021"

  #if defined(ZTS) && defined(COMPILE_DL_GPIO)
    ZEND_TSRMLS_CACHE_EXTERN()
  #endif

  BEGIN_EXTERN_C()

  /* Class entry pointers */
  extern zend_class_entry *zceChip;
  extern zend_class_entry *zceException;
  extern zend_class_entry *zceEvent;
  extern zend_class_entry *zcePin;
  extern zend_class_entry *zcePins;

  END_EXTERN_C()

#endif
