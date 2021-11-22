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

#ifndef GPIO_DEFINES_H
  #define GPIO_DEFINES_H

  /* use any uAPI GPIO ABI version available */
  #define ABI_ANY 0x00
  /* force uAPI GPIO ABI v1 (deprecated) */
  #define ABI_V1 0x01
  /* force uAPI GPIO ABI v2 */
  #define ABI_V2 0x02

  #define BIAS_DISABLED 0x00
  #define BIAS_PULL_UP 0x01
  #define BIAS_PULL_DOWN 0x02
  #define BIAS_AS_IS 0x04

  #define DRIVE_PUSH_PULL 0x01
  #define DRIVE_OPEN_DRAIN 0x02
  #define DRIVE_OPEN_SOURCE 0x03

  #define FLAG_NONE 0x00
  /* pin is an open drain output */
  #define FLAG_OPEN_DRAIN 0x01
  /* pin is an open source output */
  #define FLAG_OPEN_SOURCE 0x02
  /* pin active state is physical low */
  #define FLAG_ACTIVE_LOW 0x04
  /* pin has bias disabled */
  #define FLAG_BIAS_DISABLED 0x08
  /* pin has pull-down bias enabled */
  #define FLAG_BIAS_PULL_DOWN 0x10
  /* pin has pull-up bias enabled */
  #define FLAG_BIAS_PULL_UP 0x20
  /* pin detects falling (active to inactive) edges */
  #define FALLING_EDGE 0x01
  /* pin detects rising (inactive to active) edges */
  #define RISING_EDGE 0x02
  /* pin detects both falling and rising edges */
  #define BOTH_EDGES FALLING_EDGE | RISING_EDGE

#endif
