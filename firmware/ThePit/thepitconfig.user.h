/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

// See thepitconfig.h
namespace hid::user
{
  // How many gamepad we currently emulate.
  constexpr uint8_t GamepadCount = min(hid::AvailableGamepadCount, 2);
}

namespace hc165::user
{
  // How many hc165 chips are daisy chained.
  constexpr uint8_t ChipCount = 10;
}

namespace pit::user
{
  // Number of Knobs (analog inputs, typically potentiometers) currently connected.
  constexpr uint8_t KnobCount = 0;//min(hid::TotalAxisCount, 4);
  constexpr uint8_t analogPins[KnobCount]{};// = {A0, A1, A2, A3};
}
