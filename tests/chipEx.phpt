--TEST--
GPIO\Chip Extended Class Test
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
try {
  $chip->foo = 1;
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  $chip[0] = 1;
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  unset($chip->foo);
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  unset($chip[0]);
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Cannot create dynamic property GPIO\Chip::$foo
Cannot use object of type GPIO\Chip as array
Cannot unset GPIO\Chip::$foo property
Cannot use object of type GPIO\Chip as array
