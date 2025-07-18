#pragma once
#include "pch.h"
#define MAX_CONTROLLERS 8
enum controllerType
{
	XInput = 1,
	DS = 2,
	DS4 = 3,
	DS3 = 4,
	NT = 5,
	SDL = 6
};
struct GenericInputController
{
	WORD  wButtons;
	BYTE  bLeftTrigger;
	BYTE  bRightTrigger;
	SHORT sThumbLX;
	SHORT sThumbLY;
	SHORT sThumbRX;
	SHORT sThumbRY;
	controllerType conType;
	std::wstring BusType;
	std::wstring SerialNum;
	BTH_ADDR ullbtDeviceInfo;
	HIDD_ATTRIBUTES Atributes;
	std::wstring Path;
	std::wstring XInputPath;
	std::wstring BTPath;
	HANDLE DeviceHandle;
	DWORD InputBufferSize;
	DWORD dwPacketNumber;
};