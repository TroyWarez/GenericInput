#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <initguid.h>
#include <Usbiodef.h>
#include <ioapiset.h>
#include <hidsdi.h>
#include <Hidclass.h>
#include <dbt.h>
#include <bluetoothapis.h>
#include <Bthdef.h>
#include <setupapi.h>
#include <Cfgmgr32.h>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.foundation.collections.h>
#include <Windows.Gaming.Input.h>
#define CONTROLLER_BUTTON_A 0x1000
#define CONTROLLER_BUTTON_B 0x2000
#define CONTROLLER_BUTTON_X 0x4000
#define CONTROLLER_BUTTON_Y 0x8000
#define CONTROLLER_BUTTON_BACK 0x0020
#define CONTROLLER_BUTTON_GUIDE 0x00400
#define CONTROLLER_BUTTON_START 0x0010
#define CONTROLLER_BUTTON_LEFTSTICK 0x0040
#define CONTROLLER_BUTTON_RIGHTSTICK 0x0080
#define CONTROLLER_BUTTON_LEFTSHOULDER 0x0100
#define CONTROLLER_BUTTON_RIGHTSHOULDER 0x0200
#define CONTROLLER_BUTTON_DPAD_UP 0x0001
#define CONTROLLER_BUTTON_DPAD_DOWN 0x0002
#define CONTROLLER_BUTTON_DPAD_LEFT 0x0004
#define CONTROLLER_BUTTON_DPAD_RIGHT 0x0008
#define CONTROLLER_GAMEPAD_GUIDE 0x0400

DEFINE_GUID(GUID_DEVCLASS_XINPUT, 0xec87f1e3, 0xc13b, 0x4100, 0xb5, 0xf7, 0x8b, 0x84, 0xd5, 0x42, 0x60, 0xcb);
//{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}
DEFINE_GUID(GUID_DEVCLASS_BLUETOOTH, 0xE0CBF06C, 0xCD8B, 0x4647, 0xBB, 0x8A, 0x26, 0x3B, 0x43, 0xF0, 0xF9, 0x74);
#include "GenericInputController.h"

typedef struct _GAMEPAD_DATA {//DO NOT MODIFY, DOING SO WILL BREAK THE CONTROLLER DATABASE!
	USHORT VID;
	USHORT PID;
	std::wstring Name;
	BYTE  bLeftTrigger;
	BYTE  bRightTrigger;
	std::wstring TriggerType; // a is analog, b is digital
	SHORT sThumbLX;
	SHORT sThumbLY;
	SHORT sThumbRX;
	SHORT sThumbRY;
	BYTE bButtonA;
	BYTE bButtonB;
	BYTE bButtonX;
	BYTE bButtonY;
	BYTE bButtonBack;
	BYTE bButtonGuide;
	BYTE bButtonStart;
	BYTE bButtonLeftStick;
	BYTE bButtonRightStick;
	BYTE bButtonLeftShoulder;
	BYTE bButtonRightShoulder;
	BYTE bButtonDpadUp;
	BYTE bButtonDpadDown;
	BYTE bButtonDpadLeft;
	BYTE bButtonDpadRight;

} GAMEPAD_DATA, * P_GAMEPAD_DATA;
