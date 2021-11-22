#!/usr/bin/env php
<?php
declare(strict_types = 1);

/**
 * Ported from the original gpiodetect provided by libgpiod
 *
 * @link https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/tree/tools/gpiodetect.c
 */

if (extension_loaded('phpgpio') === false) {
  echo 'To use this tool, you must first install/enable the "phpgpio" extension!', PHP_EOL;

  return 1;
}

if ($argc > 1) {
  if (in_array($argv[1], ['-h', '--help'])) {
    echo 'Usage: ', $argv[0], ' [OPTIONS]', PHP_EOL;
    echo PHP_EOL;
    echo 'List all GPIO chips, print their labels and number of GPIO pins.', PHP_EOL;
    echo PHP_EOL;
    echo 'Options:', PHP_EOL;
    echo '  -h, --help:    display this message and exit', PHP_EOL;
    echo '  -v, --version: display the version and exit', PHP_EOL;

    return 0;
  }

  if (in_array($argv[1], ['-v', '--version'])) {
    echo basename(__FILE__), ' phpgpio/', GPIO\EXTENSION_VERSION, ' (libgpiod/', GPIO\LIBRARY_VERSION, ')', PHP_EOL;
    echo PHP_EOL;
    echo 'URL: https://github.com/flavioheleno/phpgpio', PHP_EOL;
    echo 'License: The PHP License, version 3.01', PHP_EOL;
    echo PHP_EOL;
    echo 'This is free software: you are free to change and redistribute it.', PHP_EOL;
    echo 'There is NO WARRANTY, to the extent permitted by law.', PHP_EOL;

    return 0;
  }

  echo 'Unrecognized argument: ', $argv[1], PHP_EOL;

  return 1;
}

if (is_dir('/dev') === false) {
  echo 'Unable to scan /dev', PHP_EOL;

  return 1;
}

$directory = new DirectoryIterator('/dev');
foreach ($directory as $fileInfo) {
  if ($fileInfo->getType() !== 'char') {
    continue;
  }

  if (preg_match('/^gpiochip[0-9]+$/', $fileInfo->getFilename()) === 1) {
    try {
      if ($fileInfo->isReadable() === false) {
        echo 'Chip is not readable: ', $fileInfo->getFilename(), PHP_EOL;

        continue;
      }

      $chip = new GPIO\Chip($fileInfo->getPathname());

      printf(
        '%s [%s] (%u pins)',
        $chip->getName(),
        $chip->getLabel(),
        $chip->getPinCount()
      );
      echo PHP_EOL;

    } catch (GPIO\Exception $exception) {
      echo 'Unable to open ', $fileInfo->getFilename(), PHP_EOL;
    }
  }
}

return 0;
