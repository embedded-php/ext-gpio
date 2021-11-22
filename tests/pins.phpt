--TEST--
GPIO\Pins General Class Test
--SKIPIF--
<?php
if (! extension_loaded('phpgpio')) {
  exit('skip');
}
?>
--FILE--
<?php
var_dump(is_subclass_of(GPIO\Pins::class, \Countable::class));
var_dump(is_subclass_of(GPIO\Pins::class, \ArrayAccess::class));
var_dump(is_subclass_of(GPIO\Pins::class, \Iterator::class));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
