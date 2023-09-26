/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Based on:
//   (1)
//     Demo sketch to read from a 74HC165 input shift register
//     by Nick Gammon, https://www.gammon.com.au/forum/?id=11979
//   (2)
//     74HC165 Shift register input example
//     by Uri Shaked, https://wokwi.com/arduino/projects/306031380875182657

#include "hc165.h"

#include <SPI.h>

namespace Pins {
#ifdef __AVR_ATmega32U4__
  #define LATCHDDR DDRC
  #define LATCHPORT PORTC
  constexpr byte latchBit = 1 << 6;
#else
  enum class Digital : byte {
    Latch = 5, // Latch inputs into registers.
  };
#endif
} // Pins


template<typename Pin>
constexpr byte pin_cast(Pin pin) noexcept
{
  return static_cast<byte>(pin);
}

constexpr uint32_t operator "" _MHz(unsigned long long mhz) noexcept
{
  return mhz * 1000000;
}

namespace hc165
{
  void begin()
  {
    SPI.begin();
#ifdef __AVR_ATmega32U4__
    LATCHDDR |= Pins::latchBit; // Pin <Latch> as OUTPUT.
    LATCHPORT |= Pins::latchBit; // Pin <Latch> HIGH.
#else
    // Portable.
    pinMode(pin_cast(Pins::Digital::Latch), OUTPUT);   // latch (copy input into registers), idle HIGH
    digitalWrite(pin_cast(Pins::Digital::Latch), HIGH);
#endif
  }

  void latch()
  {
    // Timing sensitive.
    noInterrupts();
    // Give a pulse to the parallel load latch of all 74HC165
#ifdef __AVR_ATmega32U4__
    LATCHPORT &= static_cast<uint8_t>(~Pins::latchBit); // Pin <Latch> LOW.
#else
    constexpr auto latchPin = pin_cast(Pins::Digital::Latch);
    digitalWrite(latchPin, LOW);
#endif
    _NOP();
    _NOP();
#ifdef __AVR_ATmega32U4__
    LATCHPORT |= Pins::latchBit; // Pin <Latch> HIGH.
#else
    digitalWrite(latchPin, HIGH);
#endif
    interrupts();
  }

  void readAll(byte chips[user::ChipCount])
  {
    // Note: If it starts misbehaving, try lowering the speed.
    SPI.beginTransaction(SPISettings{16_MHz, MSBFIRST, SPI_MODE3});
    SPI.transfer(chips, user::ChipCount);
    SPI.endTransaction();
  }
}