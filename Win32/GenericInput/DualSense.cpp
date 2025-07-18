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
	BOOL bResult = FALSE;
	HIDD_ATTRIBUTES DeviceAttrib = { 0 };
	if (HidD_GetAttributes(DeviceH, &DeviceAttrib))
	{
		PHIDP_PREPARSED_DATA PreparsedData = nullptr;
		if (HidD_GetPreparsedData(DeviceH, &PreparsedData))
		{
			HIDP_CAPS deviceCaps = { 0 };

			if (HidP_GetCaps(PreparsedData, &deviceCaps) == HIDP_STATUS_SUCCESS && deviceCaps.InputReportByteLength == Bluetooth) {
				BYTE ControllerMAC[20] = { 0 };
				ControllerMAC[0] = 0x09;
				if (HidD_GetFeature(DeviceH, ControllerMAC, sizeof(ControllerMAC)))
				{
					memcpy(&ullbtDeviceInfo, &ControllerMAC[1], 6);
					bResult = TRUE;
				}
			}
			if (HidD_FreePreparsedData(PreparsedData) == FALSE)
			{
				bResult = FALSE;
			}
		}
	}

	if (DeviceH)
	{
		CloseHandle(DeviceH);
	}
	return bResult;
}
DWORD DualSense::GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState)
{
	if (controller == nullptr || controller->DeviceHandle == 0 || controller->DeviceHandle == INVALID_HANDLE_VALUE || pState == nullptr)
	{
		return ERROR_INVALID_PARAMETER;
	}
	DWORD Flags = 0;
	if (GetHandleInformation(controller->DeviceHandle, &Flags) == FALSE)
	{
		return GetLastError();
	}
	if (controller->dwPacketNumber < 10241)
	{
		controller->dwPacketNumber++;
	}
	else
	{
		controller->dwPacketNumber = 0L;
	}
	pState->dwPacketNumber = controller->dwPacketNumber;
	switch (Bluetooth)
	{
	case Bluetooth:
	{
		InputBufferBt[0] = 0x31;
// 		if (ReadFile(controller->DeviceHandle, InputBuffer.data(), (DWORD)InputBuffer.size(), NULL, NULL) == FALSE)
// 		{
// 			return GetLastError();
// 		}
		if (HidD_GetInputReport(controller->DeviceHandle, InputBufferBt, (DWORD)sizeof(InputBufferBt)))
		{


		if (InputBufferBt[0] == 0x31)
		{
			pState->Gamepad.sThumbLX = XByteToShort[InputBufferBt[1]];
			pState->Gamepad.sThumbLY = YByteToShort[InputBufferBt[2]];
			pState->Gamepad.sThumbRX = XByteToShort[InputBufferBt[3]];
			pState->Gamepad.sThumbRY = YByteToShort[InputBufferBt[4]];

			pState->Gamepad.bLeftTrigger = InputBufferBt[6];
			pState->Gamepad.bRightTrigger = InputBufferBt[7];

			switch (InputBufferBt[9] & 0xf0)
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

			switch (InputBufferBt[9] & 0x0f)
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

			switch (InputBufferBt[10] & 0xf0)
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

			switch (InputBufferBt[10] & 0x0f)
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

			switch (InputBufferBt[11] & 0x0f)//Mic button and touch pad button need to be dealt with differently
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
		InputBufferBt[0] = 0x01;
		if (HidD_GetInputReport(controller->DeviceHandle, InputBufferUsb, (DWORD)sizeof(InputBufferUsb)))
		{
			InputBufferUsb[0] = 0x01;
			if (InputBufferUsb[0] == 0x01)
			{
				pState->Gamepad.sThumbLX = XByteToShort[InputBufferUsb[1]];
				pState->Gamepad.sThumbLY = YByteToShort[InputBufferUsb[2]];
				pState->Gamepad.sThumbRX = XByteToShort[InputBufferUsb[3]];
				pState->Gamepad.sThumbRY = YByteToShort[InputBufferUsb[4]];

				pState->Gamepad.bLeftTrigger = InputBufferUsb[5];
				pState->Gamepad.bRightTrigger = InputBufferUsb[6];

				switch (InputBufferUsb[8] & 0xf0)
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

				switch (InputBufferUsb[8] & 0x0f)
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

				switch (InputBufferUsb[9] & 0xf0)
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

				switch (InputBufferUsb[9] & 0x0f)
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

				switch (InputBufferUsb[10] & 0x0f)//Mic button and touch pad button needed to be dealt with differently
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
