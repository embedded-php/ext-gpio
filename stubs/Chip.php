<?php

namespace GPIO;

/**
 * Represents a GPIO chip.
 */
final class Chip {
  /**
   * Path to the GPIO chip device.
   *
   * @var string
   */
  private string $path;
  /**
   * The GPIO chip label.
   *
   * @var string
   */
  private string $label;
  /**
   * The GPIO chip name.
   *
   * @var string
   */
  private string $name;
  /**
   * The number of available GPIO pins.
   *
   * @var int
   */
  private int $pinCount;

  /**
   * Use any uAPI GPIO ABI version available.
   *
   * @var int
   */
  public const ABI_ANY = 0x00;
  /**
   * Use uAPI GPIO ABI v1 (deprecated).
   *
   * @var int
   */
  public const ABI_V1 = 0x01;
  /**
   * Use uAPI GPIO ABI v2.
   *
   * @var int
   */
  public const ABI_V2 = 0x02;

  /**
   * Opens the chip.
   *
   * @param string $path       Path to the GPIO chip device.
   * @param int    $abiVersion Which version of the Userspace ABI for GPIO to use.
   *
   * @return void
   */
  public function __construct(string $path, int $abiVersion = self::ABI_ANY) {}

  /**
   * Return if ABI v2 is being used.
   *
   * @return bool
   */
  public function isAbiV2(): bool {}

  /**
   * Get all pins exposed by this chip.
   *
   * @return \GPIO\Pins
   */
  public function getAllPins(): Pins {}

  /**
   * Get the pin exposed by this chip at given offset.
   *
   * @param int $offset Offset of the pin.
   *
   * @return \GPIO\Pin
   */
  public function getPin(int $offset): Pin {}

  /**
   * Get a set of pins exposed by this chip at given offsets.
   *
   * @param int[] $offsets List of pin offsets.
   *
   * @return \GPIO\Pins
   */
  public function getPins(array $offsets): Pins {}

  /**
   * Return the number of available pins.
   *
   * @return int
   */
  public function getPinCount(): int {}

  /**
   * Return the path to the GPIO chip device.
   *
   * @return string
   */
  public function getPath(): string {}

  /**
   * Return the label of the chip held by this object.
   *
   * @return string
   */
  public function getLabel(): string {}

  /**
   * Return the name of the chip held by this object.
   *
   * @return string
   */
  public function getName(): string {}
}
