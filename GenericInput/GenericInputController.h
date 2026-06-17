#pragma once
#include "pch.h"
constexpr int MAX_CONTROLLERS = 8;

enum controllerType
{
	XInput = 1,
	XInput_360 = 2,
	DS = 3,
	DS4 = 4,
	DS3 = 5,
	NT = 6,
	SDL = 7
};
struct GenericInputController
{
	WORD  wButtons = 0;
	BYTE  bLeftTrigger = 0;
	BYTE  bRightTrigger = 0;
	SHORT sThumbLX = 0;
	SHORT sThumbLY = 0;
	SHORT sThumbRX = 0;
	SHORT sThumbRY = 0;
	controllerType conType = { };
	std::wstring BusType;
	std::wstring SerialNum;
	BTH_ADDR ullbtDeviceInfo = { };
	HIDD_ATTRIBUTES Atributes = { };
	std::wstring Path;
	std::wstring XInputPath;
	std::wstring BTPath;
	HANDLE DeviceHandle = nullptr;
	DWORD InputBufferSize = 0;
	DWORD dwPacketNumber = 0;
};