/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#if !defined(USBCON)
#error "ThePit cannot be used on a non-USB capable arduino."
#endif

#include "utilities.h"
#include <USBDesc.h>

// User tweakables settings are under the `user` namespaces.
namespace hid
{
  // Max available USB device we can emulate.
  constexpr uint8_t AvailableGamepadCount = USB_ENDPOINTS - (CDC_FIRST_ENDPOINT + CDC_ENPOINT_COUNT);

  // Windows limits: 128 buttons, 8 axis per gamepad.
  constexpr uint8_t ButtonCount = 128; 
  constexpr uint8_t AxisCount = 8; // + 4 remaining, Limited by hardware.
}

namespace hc165
{
  // Number of bits (ie inputs) per chip.
  constexpr uint8_t BitsPerChip = 8;
}

// User tweakables settings are under the `user` namespaces.
#include "thepitconfig.user.h"

namespace hid
{
  constexpr uint16_t TotalButtonCount = ButtonCount * user::GamepadCount;
  constexpr uint8_t TotalAxisCount = AxisCount * user::GamepadCount;
}

namespace pit
{
  constexpr bool hasKnobs() { 
    return user::KnobCount > 0;
  }
}
