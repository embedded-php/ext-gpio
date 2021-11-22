<?php

namespace GPIO;

/**
 * Represents an Event.
 */
final class Event {
  /**
   * The event type.
   * Possible values:
   *  - RISING_EDGE
   *  - FALLING_EDGE
   *
   * @var int
   */
  private int $type;
  /**
   * The event timestamp in nanoseconds.
   *
   * @var int
   */
  private int $timestamp;
  
  /**
   * Rising edge event.
   * Value map:
   *  - ABI v2: GPIO_V2_LINE_EVENT_RISING_EDGE
   *  - ABI v1: GPIOEVENT_EVENT_RISING_EDGE
   *
   * @var int
   */
  public const RISING_EDGE = 0x01;
  /**
   * Falling edge event.
   * Value map:
   *  - ABI v2: GPIO_V2_LINE_EVENT_FALLING_EDGE
   *  - ABI v1: GPIOEVENT_EVENT_FALLING_EDGE
   *
   * @var int
   */
  public const FALLING_EDGE = 0x02;

  /**
   * Get the event type.
   * Possible return values:
   *  - RISING_EDGE
   *  - FALLING_EDGE
   * 
   * @return int
   */
  public function getType(): int {}

  /**
   * Get the event timestamp in microseconds.
   * 
   * @return int
   */
  public function getTimestamp(): int {}
}
