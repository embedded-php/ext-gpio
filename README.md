# GPIO PHP Extension

The GPIO PHP Extension provides an object-oriented wrapper to the
[gpio](https://www.kernel.org/doc/Documentation/gpio/gpio.txt) Linux Userspace ABI that gives users the ability to write
code that interacts with the [general-purpose input/output](https://en.wikipedia.org/wiki/General-purpose_input/output)
pins available in Linux-based [Single-Board Computers](https://en.wikipedia.org/wiki/Single-board_computer) such as the
[Raspberry Pi](https://en.wikipedia.org/wiki/Raspberry_Pi).

## Acknowledgements

The initial version of this extension was based on [libgpiod](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/) and still is heavily inspired by it.

## Requirements

- PHP >= 8.0 (either NTS or ZTS version);

> **Note:** if you are using this extension on Raspbian or Raspberry Pi OS, the user executing the script **must**
> belong to the **gpio** group or you may run into "Permission denied" errors.

On **Raspbian** or **Raspberry Pi OS**:

**PHP 8.0**:
```bash
sudo apt install php8.0-cli php8.0-dev php8.0-phpdbg
```

**PHP 8.1**:
```bash
sudo apt install php8.1-cli php8.1-dev php8.1-phpdbg
```

## Building

```bash
phpize
./configure
make
make test
```

## Quick start

A simple "softwire" example that reads the value from input pin #2 and writes its value to output pin #3:

```php
$chip = new GPIO\Chip('/dev/gpiochip0');
$p2 = $chip
  ->getPin(2)
  ->asInput('softwire');
$p3 = $chip
  ->getPin(3)
  ->asOutput('softwire');

$p2->isHigh() ? $p3->setHigh() : $p3->setLow();

```

## Stubs

After changing any of the [stubs](stubs/), the main [stub file](phpgpio.stub.php) must be updated by running the
command below.

```bash
php stubs/update-main-stup.php
```

Once the stub is updated, the regular build process can be executed.

> **Note:** The main stub file ([phpgpio.stub.php](phpgpio.stub.php)) is a generated file, edit the files on the
[stubs](stubs/) folder instead.

## Classes

Class                                 | Description
--------------------------------------|------------
[GPIO\Chip](stubs/Chip.php)           | Represents a GPIO chip.
[GPIO\Event](stubs/Event.php)         | Represents a GPIO event.
[GPIO\Exception](stubs/Exception.php) | GPIO base exception.
[GPIO\Pin](stub/Pin.php)              | Represents a single GPIO pin.
[GPIO\Pins](stubs/Pins.php)           | Represents a collection of GPIO pins.

## Tools

Name                                   | Description
---------------------------------------|------------
[gpiodetect.php](tools/gpiodetect.php) | List all GPIO chips, print their labels and number of GPIO pins.
[gpioinfo.php](tools/gpioinfo.php)     | Print information about all pins of the specified GPIO chip(s) (or all gpiochips if none are specified).

## Supported Hardware

The table below lists the supported and tested chips.

SoC                               | Model
----------------------------------|------
[BCM2835](tests/BCM2835.phpt)     | [Raspberry Pi 1 and Zero](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/README.md)
[BCM2836](tests/BCM2836.phpt)     | [Raspberry Pi 2](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/README.md)
[BCM2837](tests/BCM2837.phpt)     | [Raspberry Pi 3 (and later Raspberry Pi 2)](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2837/README.md)
[BCM2837B0](tests/BCM2837B0.phpt) | [Raspberry Pi 3B+ and 3A+](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2837b0/README.md)
[BCM2711](tests/BCM2711.phpt)     | [Raspberry Pi 4B](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2711/README.md)

## Alternatives

- [PiPHP/GPIO](https://github.com/PiPHP/GPIO)
- [ronanguilloux/php-gpio](https://github.com/ronanguilloux/php-gpio)
- [Volantus/php-pigpio](https://github.com/Volantus/php-pigpio)

## Other Languages

- Golang: [periph/conn](https://github.com/periph/conn)
- Golang: [warthog618/gpiod](https://github.com/warthog618/gpiod)
- Java: [mattjlewis/diozero](https://github.com/mattjlewis/diozero)
- NodeJS: [sombriks/node-libgpiod](https://github.com/sombriks/node-libgpiod)

## Resources

- https://elinux.org/images/9/9b/GPIO_for_Engineers_and_Makers.pdf
- https://github.com/raspberrypi/linux/blob/rpi-5.10.y/Documentation/devicetree/bindings/gpio/gpio.txt
- https://github.com/raspberrypi/linux/blob/rpi-5.10.y/include/uapi/linux/gpio.h
- https://www.kernel.org/doc/html/latest/driver-api/gpio/index.html
- https://www.raspberrypi.org/documentation/hardware/raspberrypi/gpio/README.md

## License

This library is licensed under the [PHP License](LICENSE).
