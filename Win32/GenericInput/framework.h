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
#include <Xinput.h>//Remove me
#include <windows.foundation.collections.h>
#include <Windows.Gaming.Input.h>
#include <vector>
#include <string>
#include <algorithm>
#define XINPUT_GAMEPAD_GUIDE 0x0400
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
