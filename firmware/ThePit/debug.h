/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#define PIT_DEBUG 0

namespace debug
{
#if PIT_DEBUG
  inline void setup()
  {
    Serial.begin( 115200);
    Serial.println( "The Pit DEBUGGER");
  }

  inline void toggled(size_t chipIndex, byte which, bool isPressed)
  {
    Serial.print( "Switch [");
    Serial.print(chipIndex);
    Serial.print("][");
    Serial.print(which);
    Serial.print( "] is now ");
    Serial.println(isPressed  ? "down ↓" : "up   ↑");
  }

  inline void println(const char* data)
  {
    Serial.println(data);
  }
#else
  inline void setup() __attribute__((always_inline));
  inline void println(...) __attribute__((always_inline));

  inline void setup() {}
  inline void println(...) {}
#endif
}
