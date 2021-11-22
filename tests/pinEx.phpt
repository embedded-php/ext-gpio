--TEST--
GPIO\Pin Extended Class Test
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
$pin = $chip->getPin(0);
try {
  $pin->foo = 1;
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  $pin[0] = 1;
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  unset($pin->foo);
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}

try {
  unset($pin[0]);
} catch (\Error $exception) {
  echo $exception->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Cannot create dynamic property GPIO\Pin::$foo
Cannot use object of type GPIO\Pin as array
Cannot unset GPIO\Pin::$foo property
Cannot use object of type GPIO\Pin as array
