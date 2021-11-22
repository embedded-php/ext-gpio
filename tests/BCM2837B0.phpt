--TEST--
BCM2837B0 Test (Raspberry Pi 3B+ and 3A+)
--SKIPIF--
<?php
if (! extension_loaded('phpgpio')) {
  exit('skip');
}

if (is_file('/sys/firmware/devicetree/base/model') === false) {
  exit('skip');
}

$model = file_get_contents('/sys/firmware/devicetree/base/model');
if (preg_match('/^Raspberry Pi 3/', $model) !== 1) {
  exit('skip');
}

if (! file_exists('/dev/gpiochip0')) {
  die('skip');
}

$chip = new GPIO\Chip('/dev/gpiochip0');
if ($chip->getLabel() !== 'pinctrl-bcm2837b0') {
  exit('skip');
}
?>
--FILE--
<?php
$chip = new GPIO\Chip('/dev/gpiochip0');
var_dump($chip->getPinCount());
$pin = $chip->getPin(0);
var_dump(get_class($pin));
var_dump($pin->getName());
$pins = $chip->getAllPins();
foreach ($pins as $l) {
  var_dump($l->getName());
}

var_dump(count($pins));
var_dump($pins[42]->getConsumer());

$pins = $chip->getPins([1, 14, 52, 7, 2, 0]);
foreach ($pins as $pin) {
  var_dump($pin->getOffset());
}
?>
--EXPECT--

