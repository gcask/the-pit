/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "thepithid.h"

#include "debug.h"
#include "descriptor.h"


extern const DeviceDescriptor deviceDescriptor PROGMEM;

// Override the defaults (mainly so we can update the version)
// 1.03: Default initial.
// 1.10: Multiple gamepads to work with windows limits.
constexpr uint16_t version = 0x0110;
const DeviceDescriptor deviceDescriptor =
	D_DEVICE(0x00,0x00,0x00,64,USB_VID,USB_PID,version,IMANUFACTURER,IPRODUCT,ISERIAL,1);

struct HIDDescriptor
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bcdHIDL;
  uint8_t bcdHIDH;
  uint8_t bCountryCode;
  uint8_t bNumDescriptors;
  uint8_t bReportDescriptorType;
  uint8_t wDescriptorLengthL;
  uint8_t wDescriptorLengthH;
};

constexpr uint8_t HID_SUBCLASS_NONE = 0;
constexpr uint8_t HID_PROTOCOL_NONE = 0;
constexpr uint8_t HID_HID_CLASS = 0x21;
constexpr uint8_t HID_REPORT_CLASS = 0x22;
constexpr uint8_t HID_COUNTRY_CODE_NONE = 0;

constexpr uint8_t HID_REQUEST_TYPE = 0x81;

constexpr uint8_t HID_GET_REPORT    = 0x01;
constexpr uint8_t HID_GET_IDLE      = 0x02;
constexpr uint8_t HID_GET_PROTOCOL  = 0x03;
constexpr uint8_t HID_SET_REPORT    = 0x09;
constexpr uint8_t HID_SET_IDLE      = 0x0A;
constexpr uint8_t HID_SET_PROTOCOL  = 0x0B;

constexpr uint8_t HID_REPORT_PROTOCOL =	1;

#define D_HID(reportLength) \
  { 0x09, HID_HID_CLASS, 0x11, 0x01, HID_COUNTRY_CODE_NONE, 1, HID_REPORT_CLASS, lowByte(reportLength), highByte(reportLength) }

// taken from Arduino's USBCore.cpp
static bool USB_SendStringDescriptor(const u8*string_P, u8 string_len, uint8_t flags) {
	auto SendControl = [](u8 data) { return USB_SendControl(0, &data, 1) == 1; };
	SendControl(2 + string_len * 2);
	SendControl(3);
	bool pgm = flags & TRANSFER_PGM;
	for(u8 i = 0; i < string_len; i++) {
			bool r = SendControl(pgm ? pgm_read_byte(&string_P[i]) : string_P[i]);
			r &= SendControl(0); // high byte
			if(!r) {
					return false;
			}
	}
	return true;
}

struct PitDescriptor
{
  InterfaceDescriptor interface;
  HIDDescriptor   hid;
  EndpointDescriptor  endpoint;
};

ThePitHID_& ThePitHID()
{
  static ThePitHID_ thepitHID;
  return thepitHID;
}

void ThePitHID_::write(uint8_t gamepad)
{
  USB_Send((pluggedEndpoint + gamepad) | TRANSFER_RELEASE, &states[gamepad], sizeof(GamepadState));
}

// Gamepads need one interface each to properly work.
ThePitHID_::ThePitHID_()
  :PluggableUSBModule(hid::user::GamepadCount, hid::user::GamepadCount, endpointTypes)
{
  for (auto& endpointType: endpointTypes) {
    // All endpoints are gamepads, using interrupt in types.
    endpointType = EP_TYPE_INTERRUPT_IN;
  }

  for (auto& state: states) {
    memset(state.buttons, 0, (hid::ButtonCount + 8 - 1) / 8);
    memset(state.axis, 0, hid::AxisCount * sizeof(int16_t));
  }

  PluggableUSB().plug(this);
}

void ThePitHID_::begin()
{
}

bool ThePitHID_::setup(USBSetup& setup)
{
  if (setup.bmRequestType != REQUEST_DEVICETOHOST_CLASS_INTERFACE) { return false; }
	if (setup.wIndex < pluggedInterface || setup.wIndex >= pluggedInterface + numInterfaces) { return false; }

  switch (setup.bRequest) {
    case HID_GET_REPORT: {
      const auto reportType = setup.wValueH;
      if (reportType != 0x01) { return false; } // Only handle Input report.

      const auto reportID = setup.wValueL;
      if (reportID != 1) { return false; }
      USB_SendControl(0, getState(setup.wIndex - pluggedInterface), sizeof(GamepadState));
      return true;
    }
    break;

    case HID_GET_PROTOCOL: {
      const auto protocol = HID_REPORT_PROTOCOL;
      USB_SendControl(0, &protocol, sizeof(protocol));
      return true;
    }
    break;

    case HID_GET_IDLE: {
      const uint8_t idling = 0; // idles indefinitely (reports only changes)
      USB_SendControl(0, &idling, sizeof(idling));
      return true;
    }
    break;
  }

  return false;
}

int ThePitHID_::getInterface(uint8_t* interfaceCount)
{
  *interfaceCount += numInterfaces;
  int sent = 0;

  PitDescriptor interfaceDesc{
    D_INTERFACE(0, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
    D_HID(sizeof(thepithidReportDescriptor)),
    D_ENDPOINT(0, USB_ENDPOINT_TYPE_INTERRUPT, min(sizeof(GamepadState), USB_EP_SIZE), 16)
  };

  for (uint8_t offset = 0; offset < numInterfaces; ++offset) {
    interfaceDesc.interface.iInterface = ISERIAL + pluggedInterface + offset;
    interfaceDesc.interface.number = pluggedInterface + offset;
    interfaceDesc.endpoint.addr = USB_ENDPOINT_IN(pluggedEndpoint + offset);
    const auto sending = USB_SendControl(0, &interfaceDesc, sizeof(interfaceDesc));
    if (sending < 0) {
      return sending;
    }

    sent += sending;
  }

	return sent;
}

int ThePitHID_::getDescriptor(USBSetup& setup)
{
  if (setup.bRequest != GET_DESCRIPTOR) { return 0; }
  const auto descriptorType = setup.wValueH;

  if(descriptorType == USB_DEVICE_DESCRIPTOR_TYPE) {
	  return USB_SendControl(TRANSFER_PGM,&deviceDescriptor,pgm_read_byte(&deviceDescriptor));
  }

  if(descriptorType == USB_STRING_DESCRIPTOR_TYPE) {
		if(setup.wValueL >= (ISERIAL + pluggedInterface) && setup.wValueL < (ISERIAL + pluggedInterface + numInterfaces)) {
			char myself[] = "The Pit A";
      myself[8] += setup.wValueL - (ISERIAL + pluggedInterface);
			return USB_SendStringDescriptor(myself, 9, 0);
		}
		
		return 0;
	}
  if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) { return 0; }
  if (setup.wIndex < pluggedInterface || setup.wIndex >= pluggedInterface + numInterfaces) { return 0; }
  

  if (descriptorType == HID_REPORT_CLASS) {
    return USB_SendControl(TRANSFER_PGM, thepithidReportDescriptor, sizeof(thepithidReportDescriptor));
  }

  if (descriptorType == HID_HID_CLASS) {
    const HIDDescriptor hidReport = D_HID(sizeof(thepithidReportDescriptor));
    return USB_SendControl(0, &hidReport, sizeof(hidReport));
  }

  
  return 0;
}

uint8_t ThePitHID_::getShortName(char *name)
{
  name[0] = 'P';
  name[1] = 'I';
  name[2] = 'T';
  return 3;
}
