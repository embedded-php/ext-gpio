<?php

namespace GPIO;

/**
 * Represents a single GPIO Pin.
 */
final class Pin {
  /**
   * Parent GPIO Chip instance.
   *
   * @var \GPIO\Chip
   */
  private Chip $chip;
  /**
   * The GPIO Pin name.
   *
   * @var string
   */
  private string $name;
  /**
   * The GPIO Pin consumer name.
   *
   * @var string
   */
  private string $consumer;
  /**
   * The GPIO Pin offset.
   *
   * @var int
   */
  private int $offset;

  /**
   * Pin's internal bias is disabled.
   *
   * @var int
   */
  public const BIAS_DISABLED = 0x00;
  /**
   * Pin's internal pull-up bias is enabled.
   *
   * @var int
   */
  public const BIAS_PULL_UP = 0x01;
  /**
   * Pin's internal pull-down bias is enabled.
   *
   * @var int
   */
  public const BIAS_PULL_DOWN = 0x02;
  /**
   * Pin's bias state is as is.
   * @var int
   */
  public const BIAS_AS_IS = 0x04;
  /**
   * Drive setting is unknown.
   *
   * @var int
   */
  public const DRIVE_PUSH_PULL = 0x01;
  /**
   * Pin output is open-drain.
   *
   * @var int
   */
  public const DRIVE_OPEN_DRAIN = 0x02;
  /**
   * Pin output is open-source.
   *
   * @var int
   */
  public const DRIVE_OPEN_SOURCE = 0x03;
  /**
   * No flags.
   *
   * @var int
   */
  public const FLAG_NONE = 0x00;
  /**
   * The pin is an open-drain port.
   *
   * @var int
   */
  public const FLAG_OPEN_DRAIN = 0x01;
  /**
   * The pin is an open-source port.
   *
   * @var int
   */
  public const FLAG_OPEN_SOURCE = 0x02;
  /**
   * The active state of the pin is LOW (HIGH is the default).
   *
   * @var int
   */
  public const FLAG_ACTIVE_LOW = 0x04;
  /**
   * The pin has neither either pull-up nor pull-down resistor.
   *
   * @var int
   */
  public const FLAG_BIAS_DISABLED = 0x08;
  /**
   * The pin has pull-down resistor enabled.
   *
   * @var int
   */
  public const FLAG_BIAS_PULL_DOWN = 0x10;
  /**
   * The pin has pull-up resistor enabled.
   *
   * @var int
   */
  public const FLAG_BIAS_PULL_UP = 0x20;
  /**
   * Falling edge event.
   *
   * @var int
   */
  public const FALLING_EDGE = 0x04;
  /**
   * Rising edge event.
   *
   * @var int
   */
  public const RISING_EDGE = 0x05;
  /**
   * Both types of edge events.
   *
   * @var int
   */
  public const BOTH_EDGES = 0x06;

  /**
   * Get current bias of this pin.
   * Possible return values:
   *  - BIAS_UNKNOWN
   *  - BIAS_DISABLED
   *  - BIAS_PULL_UP
   *  - BIAS_PULL_DOWN
   *
   * @return int
   */
  public function getBias(): int {}

  /**
   * Get the consumer name of this pin (if any).
   *
   * @return string
   */
  public function getConsumer(): string {}

  /**
   * Get current drive setting of this pin.
   * Possible return values:
   *  - DRIVE_PUSH_PULL
   *  - DRIVE_OPEN_DRAIN
   *  - DRIVE_OPEN_SOURCE
   *
   * @return int
   */
  public function getDrive(): int {}

  /**
   * Get the parent chip.
   *
   * @return \GPIO\Chip
   */
  public function getChip(): Chip {}

  /**
   * Check if this pin's signal is inverted.
   *
   * @return bool
   */
  public function isActiveLow(): bool {}

  /**
   * Check if this pin is used by the kernel or other user space process.
   *
   * @return bool
   */
  public function isUsed(): bool {}

  /**
   * Get the name of this pin (if any).
   *
   * @return string
   */
  public function getName(): string {}

  /**
   * Get the offset of this pin.
   *
   * @return int
   */
  public function getOffset(): int {}

  /**
   * Request this pin as INPUT.
   * Possible $flag values:
   *  - FLAG_NONE
   *  - FLAG_ACTIVE_LOW
   *  - FLAG_BIAS_DISABLED
   *  - FLAG_BIAS_PULL_DOWN
   *  - FLAG_BIAS_PULL_UP
   *
   * Note: only a single FLAG_BIAS_* flag can be set at a time.
   *
   * @param string $consumer       Name of the consumer.
   * @param int    $flags          Request configuration flags.
   * @param int    $debouncePeriod Number of microseconds to debounce input changes (ABI v2 only)
   *
   * @return self
   */
  public function asInput(string $consumer, int $flags = self::FLAG_NONE, int $debouncePeriod = 0): self {}

  /**
   * Request this pin as OUTPUT.
   * Possible $flag values:
   *  - FLAG_NONE
   *  - FLAG_OPEN_DRAIN
   *  - FLAG_OPEN_SOURCE
   *  - FLAG_ACTIVE_LOW
   *  - FLAG_BIAS_DISABLED
   *  - FLAG_BIAS_PULL_DOWN
   *  - FLAG_BIAS_PULL_UP
   *
   * Note: only a single FLAG_OPEN_* flag can be set at a time.
   * Note: only a single FLAG_BIAS_* flag can be set at a time.
   *
   * @param string $consumer Name of the consumer.
   * @param int    $flags    Request configuration flags.
   * @param bool   $setHigh  Set the pin value to HIGH.
   *
   * @return self
   */
  public function asOutput(string $consumer, int $flags = self::FLAG_NONE, bool $setHigh = false): self {}

  /**
   * Return if the pin is set as INPUT.
   *
   * @return bool
   */
  public function isInput(): bool {}

  /**
   * Return if the pin is set as OUTPUT.
   *
   * @return bool
   */
  public function isOutput(): bool {}

  /**
   * Return if the pin input value is HIGH.
   * Note: INPUT pin only.
   *
   * @return bool
   */
  public function isHigh(): bool {}

  /**
   * Return if the pin input value is LOW.
   * Note: INPUT pin only.
   *
   * @return bool
   */
  public function isLow(): bool {}

  /**
   * Set the output value of this pin to HIGH.
   * Note: OUTPUT pin only.
   *
   * @return self
   */
  public function setHigh(): self {}

  /**
   * Set the output value of this pin to LOW.
   * Note: OUTPUT pin only.
   *
   * @return self
   */
  public function setLow(): self {}

  /**
   * Wait for an edge change event.
   * Return an instance of \GPIO\Event if an event has occurred within the timeout interval, null otherwise.
   * Possible $edgeType values:
   * - FALLING_EDGE
   * - RISING_EDGE
   * - BOTH_EDGES
   *
   * @param int $seconds     Timeout interval in seconds.
   * @param int $nanoseconds Timeout interval in nanoseconds.
   * @param int $edgeType    Type of edge change to wait for.
   *
   * @return \GPIO\Event|null
   */
  public function waitForEdge(int $seconds, int $nanoseconds = 0, int $edgeType = self::BOTH_EDGES): ?Event {}
}
