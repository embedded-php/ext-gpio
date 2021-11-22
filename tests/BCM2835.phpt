--TEST--
BCM2835 Test (Raspberry Pi 1 and Zero)
--SKIPIF--
<?php
if (! extension_loaded('phpgpio')) {
  exit('skip');
}

if (is_file('/sys/firmware/devicetree/base/model') === false) {
  exit('skip');
}

$model = file_get_contents('/sys/firmware/devicetree/base/model');
if (preg_match('/^Raspberry Pi (Model|Zero)/', $model) !== 1) {
  exit('skip');
}

if (! file_exists('/dev/gpiochip0')) {
  die('skip');
}

$chip = new GPIO\Chip('/dev/gpiochip0');
if ($chip->getLabel() !== 'pinctrl-bcm2835') {
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

$model = file_get_contents('/sys/firmware/devicetree/base/model');
if (preg_match('/^Raspberry Pi Model [AB]/', $model) !== 1) {
  // RPi 1
  var_dump($pins[16]->getConsumer());
}

if (preg_match('/^Raspberry Pi Zero/', $model) !== 1) {
  // RPi Zero
  var_dump($pins[47]->getConsumer());
}

$pins = $chip->getPins([1, 14, 52, 7, 2, 0]);
foreach ($pins as $pin) {
  var_dump($pin->getOffset());
}
?>
--EXPECT--
int(54)
string(8) "GPIO\Pin"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
string(7) "unnamed"
int(54)
string(4) "led0"
int(1)
int(14)
int(52)
int(7)
int(2)
int(0)
