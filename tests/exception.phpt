--TEST--
GPIO\Exception General Class Test
--SKIPIF--
<?php
if (! extension_loaded('phpgpio')) {
  exit('skip');
}
?>
--FILE--
<?php
var_dump(is_subclass_of(GPIO\Exception::class, \Exception::class));
?>
--EXPECT--
bool(true)
