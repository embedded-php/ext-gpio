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

#ifndef GPIO_PIN_H
  #define GPIO_PIN_H

  #include "php.h"

  BEGIN_EXTERN_C()

  /* internal object data holder */
  typedef struct _pinObject pinObject;

  /* class registration */
  extern zend_class_entry* registerPinClass(void);

  /* handle pinObject creation */
  extern zend_object *pinCreateObject(zend_class_entry *zceClass);
  /* sets the internal pinObject data */
  extern void pinSetData(zend_object *obj, int v2, int cfd, unsigned int offset);

  END_EXTERN_C()

#endif
