<?php

namespace GPIO;

/**
 * Represents multiple GPIO pins.
 */
final class Pins implements \Countable, \ArrayAccess, \Iterator {
  /**
   * Parent chip instance.
   *
   * @var \GPIO\Chip
   */
  private Chip $chip;

  /**
   * Get the parent chip.
   *
   * @return \GPIO\Chip
   */
  public function getChip(): Chip {}

  /**
   * Returns the number of pins held by collection.
   *
   * @return int
   */
  public function count(): int {}

  /**
   * Whether or not an offset exists.
   *
   * @param mixed $offset An offset to check for.
   *
   * @return bool
   */
  public function offsetExists($offset): bool {}

  /**
   * Returns the value at specified offset.
   *
   * @param mixed $offset The offset to retrieve.
   *
   * @return mixed
   */
  public function offsetGet($offset) {}

  /**
   * Assigns a value to the specified offset.
   * Note: this method throws an exception, it is declared as part of \ArrayAccess interface.
   *
   * @param mixed $offset The offset to assign the value to.
   * @param mixed $value  The value to set.
   *
   * @return void
   */
  public function offsetSet($offset, $value) {}

  /**
   * Unsets an offset.
   * Note: this method throws an exception, it is declared as part of \ArrayAccess interface.
   *
   * @param mixed $offset The offset to unset.
   *
   * @return void
   */
  public function offsetUnset($offset) {}

  /**
   * Returns the current element.
   *
   * @return mixed
   */
  public function current() {}

  /**
   * Returns the key of the current element.
   *
   * @return mixed
   */
  public function key() {}

  /**
   * Moves the current position to the next element.
   *
   * @return void
   */
  public function next(): void {}

  /**
   * Rewinds back to the first element of the Iterator.
   *
   * @return void
   */
  public function rewind(): void {}

  /**
   * Check if the current position is valid.
   *
   * @return bool
   */
  public function valid(): bool {}
}
