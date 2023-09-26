/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

static_assert(LOW == 0, "LOW != 0");
static_assert(HIGH == 1, "HIGH != 1");

#include "debug.h"
#include "thepit.h"

// !! CDC IS DISABLED, don't forget to upload via the reset pin!
void setup()
{
  hc165::begin();
  Pit.begin();
}

void loop ()
{
  Pit.update();
}
