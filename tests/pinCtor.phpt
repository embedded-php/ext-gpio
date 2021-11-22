--TEST--
GPIO\Pin Constructor Restriction Test
--SKIPIF--
<?php
if (! extension_loaded('phpgpio')) {
  exit('skip');
}
?>
--FILE--
<?php
try {
  new GPIO\Pin();
} catch (\Error $exception) {
  echo $exception->getMessage();
}
?>
--EXPECT--
Cannot directly construct GPIO\Pin
