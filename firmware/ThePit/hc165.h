/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "utilities.h"
#include "thepitconfig.h"

namespace hc165
{
  // Sets up hc165 pins (see utilities.h)
  void begin();

  // Latches the hc165s to load parallel values.
  void latch();

  // Reads all available bytes in one pass.
  void readAll(byte chips[user::ChipCount]);
} // ns hc165
