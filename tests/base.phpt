--TEST--
Check if constants and classes are defined
--SKIPIF--
<?php
if (! extension_loaded('phpgpio')) {
  exit('skip');
}
?>
--FILE--
<?php
// Extension Constants
var_dump(defined('GPIO\\EXTENSION_VERSION'));

// Class Constants
var_dump(defined('GPIO\\Event::FALLING_EDGE'));
var_dump(defined('GPIO\\Event::RISING_EDGE'));
var_dump(defined('GPIO\\Pin::BIAS_DISABLED'));
var_dump(defined('GPIO\\Pin::BIAS_PULL_DOWN'));
var_dump(defined('GPIO\\Pin::BIAS_PULL_UP'));
var_dump(defined('GPIO\\Pin::BIAS_AS_IS'));
var_dump(defined('GPIO\\Pin::DRIVE_OPEN_DRAIN'));
var_dump(defined('GPIO\\Pin::DRIVE_OPEN_SOURCE'));
var_dump(defined('GPIO\\Pin::DRIVE_PUSH_PULL'));
var_dump(defined('GPIO\\Pin::REQUEST_EVENT_BOTH_EDGES'));
var_dump(defined('GPIO\\Pin::REQUEST_EVENT_FALLING_EDGE'));
var_dump(defined('GPIO\\Pin::REQUEST_EVENT_RISING_EDGE'));
var_dump(defined('GPIO\\Pin::REQUEST_FLAG_ACTIVE_LOW'));
var_dump(defined('GPIO\\Pin::REQUEST_FLAG_BIAS_DISABLED'));
var_dump(defined('GPIO\\Pin::REQUEST_FLAG_BIAS_PULL_DOWN'));
var_dump(defined('GPIO\\Pin::REQUEST_FLAG_BIAS_PULL_UP'));
var_dump(defined('GPIO\\Pin::REQUEST_FLAG_OPEN_DRAIN'));
var_dump(defined('GPIO\\Pin::REQUEST_FLAG_OPEN_SOURCE'));

// Classes
var_dump(class_exists('GPIO\\Chip'));
var_dump(class_exists('GPIO\\Event'));
var_dump(class_exists('GPIO\\Exception'));
var_dump(class_exists('GPIO\\Pin'));
var_dump(class_exists('GPIO\\Pins'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
