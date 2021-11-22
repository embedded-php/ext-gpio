--TEST--
GPIO\Pins Extended Class Test
--SKIPIF--
<?php
if (! extension_loaded('phpgpio')) {
  die('skip');
}

if (! file_exists('/dev/gpiochip0')) {
  die('skip');
}
?>
--FILE--
<?php
$chip = new GPIO\Chip('/dev/gpiochip0');
$bulk = $chip->getAllPins();
try {
  $bulk->foo = 1;
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  $bulk[0] = 1;
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  unset($bulk->foo);
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  unset($bulk[0]);
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Cannot create dynamic property GPIO\Pins::$foo
Cannot set GPIO\Pins pin at offset 0
Cannot unset GPIO\Pins::$foo property
Cannot unset GPIO\Pins pin at offset 0
