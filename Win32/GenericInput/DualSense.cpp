#include "pch.h"
#include "DualSense.h"
#include "CRC32.h"

#define PS_OUTPUT_CRC32_SEED 0xA2
const USHORT DualSenseVID = 0x054c;
const USHORT DualSensePID = 0x0ce6;
BOOL DualSense::GetWiredMAC(std::wstring& DevicePath, BTH_ADDR& ullbtDeviceInfo)
{
	HANDLE DeviceH = CreateFile(DevicePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, NULL, NULL);
	if (DeviceH == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	HIDD_ATTRIBUTES DeviceAttrib = { 0 };
	if (HidD_GetAttributes(DeviceH, &DeviceAttrib) == FALSE || (DeviceAttrib.VendorID != DualSenseVID  && DeviceAttrib.ProductID != DualSensePID))
	{
		return FALSE;
	}
	PHIDP_PREPARSED_DATA PreparsedData = nullptr;
	if (HidD_GetPreparsedData(DeviceH, &PreparsedData) == FALSE)
	{
		return FALSE;
	}
	HIDP_CAPS deviceCaps = { 0 };

	if (HidP_GetCaps(PreparsedData, &deviceCaps) == HIDP_STATUS_SUCCESS && deviceCaps.InputReportByteLength != 64) {
		return FALSE;
	}
	if (PreparsedData)
	{
		HidD_FreePreparsedData(PreparsedData);
	}
	BYTE ControllerMAC[20] = { 0 };
	ControllerMAC[0] = 0x09;
	if (HidD_GetFeature(DeviceH, ControllerMAC, 20) == FALSE)
	{
		return FALSE;
	}
	if (DeviceH)
	{
		CloseHandle(DeviceH);
	}
	memcpy(&ullbtDeviceInfo, &ControllerMAC[1], 6);
	return TRUE;
}
DWORD DualSense::GetState(GenericInputController& controller, GENERIC_INPUT_STATE* pState)
{
	if (controller.DeviceHandle == 0 || controller.DeviceHandle == INVALID_HANDLE_VALUE || pState == nullptr)
	{
		return ERROR_INVALID_PARAMETER;
	}
	DWORD Flags = 0;
	if (GetHandleInformation(controller.DeviceHandle, &Flags) == FALSE)
	{
		return GetLastError();
	}
	if (controller.dwPacketNumber < 10241)
	{
		controller.dwPacketNumber++;
	}
	else
	{
		controller.dwPacketNumber = 0L;
	}
	pState->dwPacketNumber = controller.dwPacketNumber;
	if (controller.InputBufferSize == 0)
	{
		PHIDP_PREPARSED_DATA pData = nullptr;
		HIDP_CAPS deviceCaps = { 0 };
		controller.InputBuffer.clear();
		if (HidD_GetPreparsedData(controller.DeviceHandle, &pData) == FALSE)
		{
			return GetLastError();
		}
		if (HidP_GetCaps(pData, &deviceCaps) != HIDP_STATUS_SUCCESS)
		{
			return GetLastError();
		}
		if (pData)
		{
			HidD_FreePreparsedData(pData);
		}

		controller.InputBufferSize = deviceCaps.InputReportByteLength;
	}

	switch ((connectionType)controller.InputBufferSize)
	{
	case Bluetooth:
	{
		if (controller.InputBuffer.size() != controller.InputBufferSize)
		{
			try {
				controller.InputBuffer.resize(controller.InputBufferSize);
				controller.InputBuffer[0] = 0x31;
			}
			catch (std::bad_alloc)
			{
				return ERROR_GEN_FAILURE;
			}
		}
// 		if (ReadFile(controller.DeviceHandle, controller.InputBuffer.data(), (DWORD)controller.InputBuffer.size(), NULL, NULL) == FALSE)
// 		{
// 			return GetLastError();
// 		}
		if (HidD_GetInputReport(controller.DeviceHandle, controller.InputBuffer.data(), (DWORD)controller.InputBufferSize))
		{


		if (controller.InputBuffer[0] == 0x31)
		{
			pState->Gamepad.sThumbLX = XByteToShort[controller.InputBuffer[1]];
			pState->Gamepad.sThumbLY = YByteToShort[controller.InputBuffer[2]];
			pState->Gamepad.sThumbRX = XByteToShort[controller.InputBuffer[3]];
			pState->Gamepad.sThumbRY = YByteToShort[controller.InputBuffer[4]];

			pState->Gamepad.bLeftTrigger = controller.InputBuffer[6];
			pState->Gamepad.bRightTrigger = controller.InputBuffer[7];

			switch (controller.InputBuffer[9] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_X;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[9] & 0x0f)
			{
			case 0x0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT | CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT | CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x4:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT | CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT | CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			}

			switch (controller.InputBuffer[10] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK | CONTROLLER_BUTTON_START;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[10] & 0x0f)
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x9:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xA:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0xB:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xD:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xE:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0xF:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[11] & 0x0f)//Mic button and touch pad button need to be dealt with differently
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x2:
			{
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x4:
			{
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x6:
			{
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			}

			break;
		}
				}
		break;
	}
	case USB:
	{
		if (controller.InputBuffer.size() != controller.InputBufferSize)
		{
			try {
				controller.InputBuffer.resize(controller.InputBufferSize);
				controller.InputBuffer[0] = 0x01;
			}
			catch (std::bad_alloc)
			{
				return ERROR_GEN_FAILURE;
			}
		}
		if (!HidD_GetInputReport(controller.DeviceHandle, controller.InputBuffer.data(), (DWORD)controller.InputBufferSize))
		{
			if (ReadFile(controller.DeviceHandle, controller.InputBuffer.data(), (DWORD)controller.InputBuffer.size(), NULL, NULL) == FALSE)
			{
				return GetLastError();
			}
		}

		if (controller.InputBuffer[0] == 0x01)
		{
			pState->Gamepad.sThumbLX = XByteToShort[controller.InputBuffer[1]];
			pState->Gamepad.sThumbLY = YByteToShort[controller.InputBuffer[2]];
			pState->Gamepad.sThumbRX = XByteToShort[controller.InputBuffer[3]];
			pState->Gamepad.sThumbRY = YByteToShort[controller.InputBuffer[4]];

			pState->Gamepad.bLeftTrigger = controller.InputBuffer[5];
			pState->Gamepad.bRightTrigger = controller.InputBuffer[6];

			switch (controller.InputBuffer[8] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_X;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
				break;
			}

			switch (controller.InputBuffer[8] & 0x0f)
			{
			case 0x0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT | CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT | CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x4:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT | CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT | CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			}

			switch (controller.InputBuffer[9] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK | CONTROLLER_BUTTON_START;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[9] & 0x0f)
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x9:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xA:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0xB:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xD:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xE:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0xF:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[10] & 0x0f)//Mic button and touch pad button needed to be dealt with differently
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x2:
			{
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x4:
			{
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x6:
			{
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			}

			break;
		}

		break;
	}
	default:
	{
		return ERROR_GEN_FAILURE;
	}
	}
	return ERROR_SUCCESS;
}
DWORD DualSense::SetState(GenericInputController& controller, LPVOID* pData, DWORD dSize)
{
	if (dSize > 512)
	{
		return ERROR_INVALID_PARAMETER;
	}
	try {
		OutputBuffer.resize(dSize);
	}
	catch (std::bad_alloc)
	{
		return ERROR_OUTOFMEMORY;
	}

	CopyMemory(OutputBuffer.data(), pData, dSize);
	uint32_t CRC32 = GetCRC32(OutputBuffer, CRC32_SEED);

	return ERROR_SUCCESS;
}
