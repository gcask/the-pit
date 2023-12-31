/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
// AUTO-GENERATED by WaratahCmd.exe

#include <stdint.h>

// HID Usage Tables: 1.3.0
// Descriptor size: 60 (bytes)
// +----------+-------+------------------+
// | ReportId | Kind  | ReportSizeInBits |
// +----------+-------+------------------+
// |        1 | Input |              256 |
// +----------+-------+------------------+
extern const uint8_t thepithidReportDescriptor[] PROGMEM;
const uint8_t thepithidReportDescriptor [] PROGMEM = 
{
    0x05, 0x01,          // UsagePage(Generic Desktop[1])
    0x09, 0x04,          // UsageId(Joystick[4])
    0xA1, 0x01,          // Collection(Application)
    0x85, 0x01,          //     ReportId(1)
    0x05, 0x09,          //     UsagePage(Button[9])
    0x19, 0x01,          //     UsageIdMin(Button 1[1])
    0x29, 0x80,          //     UsageIdMax(Button 128[128])
    0x15, 0x00,          //     LogicalMinimum(0)
    0x25, 0x01,          //     LogicalMaximum(1)
    0x95, 0x80,          //     ReportCount(128)
    0x75, 0x01,          //     ReportSize(1)
    0x81, 0x02,          //     Input(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, BitField)
    0x05, 0x01,          //     UsagePage(Generic Desktop[1])
    0x09, 0x01,          //     UsageId(Pointer[1])
    0xA1, 0x00,          //     Collection(Physical)
    0x09, 0x30,          //         UsageId(X[48])
    0x09, 0x31,          //         UsageId(Y[49])
    0x09, 0x32,          //         UsageId(Z[50])
    0x09, 0x33,          //         UsageId(Rx[51])
    0x09, 0x34,          //         UsageId(Ry[52])
    0x09, 0x35,          //         UsageId(Rz[53])
    0x09, 0x37,          //         UsageId(Dial[55])
    0x09, 0x36,          //         UsageId(Slider[54])
    0x16, 0x00, 0x00,    //         LogicalMinimum(0)
    0x26, 0xFF, 0x07,    //         LogicalMaximum(2,047)
    0x95, 0x08,          //         ReportCount(8)
    0x75, 0x10,          //         ReportSize(16)
    0x81, 0x02,          //         Input(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, BitField)
    0xC0,                //     EndCollection()
    0xC0,                // EndCollection()
};