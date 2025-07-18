#include "pch.h"
#include "DualShock4.h"
BOOL DualShock4::IsDualshock4Connected(std::wstring& DevicePath)
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
		if (DeviceAttrib.VendorID != DualShock4DongleVID || (DeviceAttrib.ProductID != DualShock4DonglePID && DeviceAttrib.ProductID != DualShock4DongleDFUPID))
		{
			return TRUE;
		}
		else if (DeviceAttrib.VendorID == DualShock4DongleVID && DeviceAttrib.ProductID == DualShock4DonglePID || (DeviceAttrib.VendorID == DualShock4DongleVID && DeviceAttrib.ProductID == DualShock4DongleDFUPID))
		{
			_HIDP_PREPARSED_DATA* PreparsedData = nullptr;
			if (HidD_GetPreparsedData(DeviceH, &PreparsedData))
			{
				HIDP_CAPS deviceCaps = { 0 };

				if (HidP_GetCaps(PreparsedData, &deviceCaps) == HIDP_STATUS_SUCCESS && deviceCaps.InputReportByteLength == USB_Dongle) {

					BYTE ControllerVID_PID[5] = { 0 };
					ControllerVID_PID[0] = 0xE3;
					if (HidD_GetFeature(DeviceH, ControllerVID_PID, sizeof(ControllerVID_PID)))
					{
						const USHORT* VID = (USHORT*)&ControllerVID_PID[1];
						const USHORT* PID = (USHORT*)&ControllerVID_PID[3];
						if (*(VID) != 0 || *(PID) != 0)
						{
							bResult = TRUE;
						}
					}
				}
				if (HidD_FreePreparsedData(PreparsedData) == FALSE)
				{
					bResult = FALSE;
				}
			}
		}
	}
	if (DeviceH)
	{
		CloseHandle(DeviceH);
	}
	return bResult;
}
DWORD DualShock4::GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState)
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

	PHIDP_PREPARSED_DATA pData = { 0 };
	HIDP_CAPS deviceCaps = { 0 };

	if (HidD_GetPreparsedData(controller->DeviceHandle, &pData) == FALSE)
	{
		return GetLastError();
	}
	if (HidP_GetCaps(pData, &deviceCaps) != HIDP_STATUS_SUCCESS)
	{
		return GetLastError();
	}
	if (HidD_FreePreparsedData(pData) == FALSE)
	{
		return GetLastError();
	}

	switch ((connectionType)deviceCaps.InputReportByteLength)
	{
	case Bluetooth:
	{
		if (!HidD_GetInputReport(controller->DeviceHandle, InputBufferBt, sizeof(InputBufferBt)))
		{
			return GetLastError();
		}

		if (InputBufferBt[0] == 0x01)
		{
				pState->Gamepad.sThumbLX = XByteToShort[InputBufferBt[1]];
				pState->Gamepad.sThumbLY = YByteToShort[InputBufferBt[2]];
				pState->Gamepad.sThumbRX = XByteToShort[InputBufferBt[3]];
				pState->Gamepad.sThumbRY = YByteToShort[InputBufferBt[4]];

				switch (InputBufferBt[5] & 0xf0)
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

				switch (InputBufferBt[5] & 0x0f)
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


				pState->Gamepad.bLeftTrigger = InputBufferBt[8];
				pState->Gamepad.bRightTrigger = InputBufferBt[9];

				switch (InputBufferBt[6] & 0xf0)
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

				switch (InputBufferBt[6] & 0x0f)
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
				case 0x4:
				{
					if (!pState->Gamepad.bLeftTrigger)
					{
						pState->Gamepad.bLeftTrigger = 0xff;
					}
					break;
				}
				case 0x5:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
					if (!pState->Gamepad.bLeftTrigger)
					{
						pState->Gamepad.bLeftTrigger = 0xff;
					}
					break;
				}
				case 0x6:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
					if (!pState->Gamepad.bLeftTrigger)
					{
						pState->Gamepad.bLeftTrigger = 0xff;
					}
					break;
				}
				case 0x7:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
					if (!pState->Gamepad.bLeftTrigger)
					{
						pState->Gamepad.bLeftTrigger = 0xff;
					}
					break;
				}
				case 0x8:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
					if (!pState->Gamepad.bLeftTrigger)
					{
						pState->Gamepad.bLeftTrigger = 0xff;
					}
					break;
				}
				case 0x9:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
					if (!pState->Gamepad.bRightTrigger)
					{
						pState->Gamepad.bRightTrigger = 0xff;
					}
					break;
				}
				case 0xA:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
					if (!pState->Gamepad.bRightTrigger)
					{
						pState->Gamepad.bRightTrigger = 0xff;
					}
					break;
				}
				case 0xB:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
					if (!pState->Gamepad.bRightTrigger)
					{
						pState->Gamepad.bRightTrigger = 0xff;
					}
					break;
				}
				case 0xC:
				{
					if (!pState->Gamepad.bRightTrigger)
					{
						pState->Gamepad.bRightTrigger = 0xff;
					}
					pState->Gamepad.bLeftTrigger = 0xff;
					break;
				}
				case 0xD:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
					if (!pState->Gamepad.bRightTrigger)
					{
						pState->Gamepad.bRightTrigger = 0xff;
					}
					if (!pState->Gamepad.bLeftTrigger)
					{
						pState->Gamepad.bLeftTrigger = 0xff;
					}
					break;
				}
				case 0xE:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
					if (!pState->Gamepad.bRightTrigger)
					{
						pState->Gamepad.bRightTrigger = 0xff;
					}
					if (!pState->Gamepad.bLeftTrigger)
					{
						pState->Gamepad.bLeftTrigger = 0xff;
					}
					break;
				}
				case 0xF:
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
					if (!pState->Gamepad.bRightTrigger)
					{
						pState->Gamepad.bRightTrigger = 0xff;
					}
					if (!pState->Gamepad.bLeftTrigger)
					{
						pState->Gamepad.bLeftTrigger = 0xff;
					}
					break;
				}
				break;
				}

				switch (InputBufferBt[7] & 0x0f)
				{
				case 0x1:// The home button was pressed
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
					break;
				}
				case 0x2:// The touch pad button was pressed
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START;
					break;
				}
				case 0x3:// The touch pad and home button were pressed
				{
					pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE | CONTROLLER_BUTTON_START;
					break;
				}
				}

				break;
			}

		break;
	}
	case USB_Dongle:
	{
		if (IsDualshock4Connected(controller->Path) == FALSE)
		{
			return ERROR_SUCCESS;
		}
		ZeroMemory(InputBufferUsb, sizeof(InputBufferUsb));
		if (ReadFile(controller->DeviceHandle, InputBufferUsb, sizeof(InputBufferUsb), NULL, NULL) == FALSE)
		{
			return GetLastError();
		}

		if (InputBufferUsb[0] == 0x01)
		{
			pState->Gamepad.sThumbLX = XByteToShort[InputBufferUsb[1]];
			pState->Gamepad.sThumbLY = YByteToShort[InputBufferUsb[2]];
			pState->Gamepad.sThumbRX = XByteToShort[InputBufferUsb[3]];
			pState->Gamepad.sThumbRY = YByteToShort[InputBufferUsb[4]];

			switch (InputBufferUsb[5] & 0xf0)
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

			switch (InputBufferUsb[5] & 0x0f)
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


			pState->Gamepad.bLeftTrigger = InputBufferUsb[8];
			pState->Gamepad.bRightTrigger = InputBufferUsb[9];

			switch (InputBufferUsb[6] & 0xf0)
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

			switch (InputBufferUsb[6] & 0x0f)
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
			case 0x4:
			{
				if (!pState->Gamepad.bLeftTrigger)
				{
					pState->Gamepad.bLeftTrigger = 0xff;
				}
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				if (!pState->Gamepad.bLeftTrigger)
				{
					pState->Gamepad.bLeftTrigger = 0xff;
				}
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				if (!pState->Gamepad.bLeftTrigger)
				{
					pState->Gamepad.bLeftTrigger = 0xff;
				}
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				if (!pState->Gamepad.bLeftTrigger)
				{
					pState->Gamepad.bLeftTrigger = 0xff;
				}
				break;
			}
			case 0x8:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				if (!pState->Gamepad.bLeftTrigger)
				{
					pState->Gamepad.bLeftTrigger = 0xff;
				}
				break;
			}
			case 0x9:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				if (!pState->Gamepad.bRightTrigger)
				{
					pState->Gamepad.bRightTrigger = 0xff;
				}
				break;
			}
			case 0xA:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				if (!pState->Gamepad.bRightTrigger)
				{
					pState->Gamepad.bRightTrigger = 0xff;
				}
				break;
			}
			case 0xB:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				if (!pState->Gamepad.bRightTrigger)
				{
					pState->Gamepad.bRightTrigger = 0xff;
				}
				break;
			}
			case 0xC:
			{
				if (!pState->Gamepad.bRightTrigger)
				{
					pState->Gamepad.bRightTrigger = 0xff;
				}
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0xD:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				if (!pState->Gamepad.bRightTrigger)
				{
					pState->Gamepad.bRightTrigger = 0xff;
				}
				if (!pState->Gamepad.bLeftTrigger)
				{
					pState->Gamepad.bLeftTrigger = 0xff;
				}
				break;
			}
			case 0xE:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				if (!pState->Gamepad.bRightTrigger)
				{
					pState->Gamepad.bRightTrigger = 0xff;
				}
				if (!pState->Gamepad.bLeftTrigger)
				{
					pState->Gamepad.bLeftTrigger = 0xff;
				}
				break;
			}
			case 0xF:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				if (!pState->Gamepad.bRightTrigger)
				{
					pState->Gamepad.bRightTrigger = 0xff;
				}
				if (!pState->Gamepad.bLeftTrigger)
				{
					pState->Gamepad.bLeftTrigger = 0xff;
				}
				break;
			}
			break;
			}

			switch (InputBufferUsb[7] & 0x0f)
			{
			case 0x1:// The home button was pressed
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x2:// The touch pad button was pressed
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START;
				break;
			}
			case 0x3:// The touch pad and home button were pressed
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE | CONTROLLER_BUTTON_START;
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