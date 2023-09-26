/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "utilities.h"
#include "PluggableUSB.h"

#include "thepitconfig.h"

struct [[gnu::packed]] GamepadState
{
  const uint8_t reportId = 1;
  uint8_t buttons[(hid::ButtonCount + 8 - 1) / 8]; // bitfields
  int16_t	axis[hid::AxisCount];

  // https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
  void update(uint8_t button, bool state) { buttons[button / 8] ^= (-state ^ buttons[button / 8]) & (1 << (button % 8)); }
};

static_assert(sizeof(GamepadState) < USB_EP_SIZE, "unexpected size");

class ThePitHID_ final : public PluggableUSBModule
{
public:
  ThePitHID_();
  void begin();
  void write(uint8_t gamepad);
  GamepadState* getState(uint8_t gamepad) { return &states[gamepad]; }
protected:
  // Overrides from PluggableUSBModule 
  bool setup(USBSetup& setup) override;
  int getInterface(uint8_t* interfaceCount) override;
  int getDescriptor(USBSetup& setup) override;
  uint8_t getShortName(char *name) override;

private:
  uint8_t endpointTypes[hid::user::GamepadCount];
  GamepadState states[hid::user::GamepadCount];
};

ThePitHID_& ThePitHID();

