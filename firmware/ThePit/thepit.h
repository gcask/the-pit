/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "utilities.h"
#include "thepithid.h"
#include "hc165.h"

class ThePit_ {
public:
  class State {
    static constexpr uint8_t currentStateMask = 0x80;
    static constexpr uint8_t debouncingMask = static_cast<uint8_t>(~currentStateMask);

    // Initialize as high (unpressed), and bouncy.
    uint8_t debounce = 0b10101010;
  public:
    constexpr bool isDown() const {
      // Pull ups: pressed is LOW state.
      return (debounce & currentStateMask) == 0;
    }

    constexpr bool isDebounced() const {
      // We're debounced if all bits under the mask are the same.
      return (debounce & debouncingMask) == debouncingMask || (debounce & debouncingMask) == 0;
    }

    void forceDownNoDebounce() {
      // Clear the high bit, marking the state as down.
      // Leave the bounce state as-is.
      debounce &= debouncingMask;
    }

    bool changed(uint8_t state) {
      auto flipped = false;
      // Mask so we don't overwrite our state bit.
      const auto currentStateBit = (debounce & currentStateMask); 
      debounce = currentStateBit | ((debounce << 1) & debouncingMask) | state;
      if (isDebounced()) {
        // Check if the current state changed.
        state <<= 7; // Move current state to high bit.
        flipped = state != currentStateBit; 
        if (flipped) {
          // On change:
          // * flip the state of the highest bit
          // * Keep the rest of the debouncing state.
          debounce = state | (debounce & debouncingMask);
        }
      }

      return flipped;
    }
  };

  ThePit_()
  {
    // According to arduino doc, this need to happen before main.
    // Because USB init.
    ThePitHID().begin();
  }

  

  void update(uint16_t which, bool state) {
    gamepad(gamepadIndex(which))->update(which % hid::ButtonCount, state);
  }
  
  void press(uint16_t which) {
    update(which, true);
  }

  void release(uint16_t which) {
    update(which, false);
  }

  void begin() {
  }

  constexpr static uint8_t gamepadIndex(uint16_t which) {
    return which / hid::ButtonCount;
  }
  
  static GamepadState* gamepad(uint8_t gamepadIndex) {
    return ThePitHID().getState(gamepadIndex);
  }

  uint8_t updateKnobs() {
    uint8_t dirtyGamepads = 0;

    if constexpr (pit::hasKnobs()) {
      auto* theGamepad = gamepad(0);
      for (uint8_t i = 0; i < pit::user::KnobCount; ++i) {
        // BUG: Why is this seemingly 12-bit?
        const int16_t updated = analogRead(pit::user::analogPins[i]); //map(analogRead(pit::analogPins[i]), 0, 1023, -32768, 32767);
        const auto changed = abs(theGamepad->axis[i] - updated) > 10;
        if (changed) {
          theGamepad->axis[i] = updated;
          dirtyGamepads = 1;
        }
      }
    }

    return dirtyGamepads;
  }

  uint8_t updateChips() {
    uint8_t dirtyGamepads = 0;

    byte chipBytes[hc165::user::ChipCount];
    hc165::readAll(chipBytes);

    // Reverse traversal, so chips at the end of the daisy chain come last.
    for (uint16_t chip = 0u; chip < hc165::user::ChipCount; ++chip) {
      const auto current = chipBytes[hc165::user::ChipCount - chip - 1];

      for (uint8_t bit = 0u; bit < hc165::BitsPerChip; ++bit) {
        const uint16_t which = chip * hc165::BitsPerChip + bit;
        auto& state = states[which];
        if (state.changed(bitRead(current, bit))) {
          update(which, state.isDown());

          // flag gamepad as dirty for write.
          bitSet(dirtyGamepads, gamepadIndex(which));
        }
      }
    }

    return dirtyGamepads;
  }

  uint16_t update() {
    constexpr auto minDelayUs = 4000;
    auto startUs = micros();
    if (lastUpdatedUs > startUs) {
      // Overflow
      lastUpdatedUs = startUs;
    }

    if (startUs - lastUpdatedUs < minDelayUs) {
      return 0;
    }
    
    // Load parallel values.
    hc165::latch();

    auto dirtyGamepads = updateKnobs() | updateChips();
    for (uint8_t i = 0; dirtyGamepads; ++i) {
      if (bitRead(dirtyGamepads, i)) {
        ThePitHID().write(i);
        bitClear(dirtyGamepads, i);
      }
    }

    const auto endUs = micros();

    if (endUs < startUs) {
      // rolled over
      startUs = endUs;
    }

    const auto elapsedUs = endUs - startUs;

    lastUpdatedUs = endUs;
    return elapsedUs < minDelayUs ? minDelayUs - elapsedUs : 0;
  }

private:
  State states[hid::TotalButtonCount];
  uint16_t lastUpdatedUs = 0;
};

extern ThePit_ Pit;
