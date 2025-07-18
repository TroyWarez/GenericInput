#include "pch.h"
#include "ProController.h"
DWORD ProController::GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState)
{
	if (controller->DeviceHandle == 0 || controller->DeviceHandle == INVALID_HANDLE_VALUE || pState == nullptr)
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
	_HIDP_PREPARSED_DATA* pData = { 0 };
	HIDP_CAPS deviceCaps = { 0 };

	if (HidD_GetPreparsedData(controller->DeviceHandle, &pData) == FALSE)
	{
		return GetLastError();
	}
	if (HidP_GetCaps(pData, &deviceCaps) != HIDP_STATUS_SUCCESS)
	{
		HidD_FreePreparsedData(pData);
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
		if (ReadFile(controller->DeviceHandle, InputBufferBt, sizeof(InputBufferBt), NULL, NULL) == FALSE)
		{
			return GetLastError();
		}

		if (InputBufferBt[0] == 0x3f)
		{
			pState->Gamepad.sThumbLX = XByteToShort[InputBufferBt[11]];
			pState->Gamepad.sThumbLY = YByteToShort[InputBufferBt[11]];
			pState->Gamepad.sThumbRX = XByteToShort[InputBufferBt[3]];
			pState->Gamepad.sThumbRY = YByteToShort[InputBufferBt[4]];

			switch (InputBufferBt[1] & 0x0f)
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_B;
				break;
			}
			case 0x4:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_X;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_A;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_A;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x8:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y;
				break;
			}
			case 0x9:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B;
				break;
			}
			case 0xA:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xB:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xC:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xD:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_B;
				break;
			}
			case 0xE:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xF:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			break;
			}

			switch (InputBufferBt[1] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.bRightTrigger = 0xff;
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				pState->Gamepad.bRightTrigger = 0xff;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				pState->Gamepad.bRightTrigger = 0xff;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				pState->Gamepad.bRightTrigger = 0xff;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.bRightTrigger = 0xff;
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				pState->Gamepad.bRightTrigger = 0xff;
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				pState->Gamepad.bRightTrigger = 0xff;
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				pState->Gamepad.bRightTrigger = 0xff;
				pState->Gamepad.bLeftTrigger = 0xff;
				break;
			}
			break;
			}

			switch (InputBufferBt[2] & 0x0f)
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_BACK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0x4:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0x8:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_RIGHTSTICK;
				break;
			}
			break;
			}

			switch (InputBufferBt[2] & 0xf0)
			{
			case 0x10:// The home button was pressed
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x20:// The screenshot button was pressed
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START;
				break;
			}
			case 0x30:// The screenshot and home button were pressed
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE | CONTROLLER_BUTTON_START;
				break;
			}
			break;
			}

			switch (InputBufferBt[3] & 0x0f)
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

			break;
		}

		break;
	}
	case USB: //Untested
	{
		if (ReadFile(controller->DeviceHandle, InputBufferUsb, sizeof(InputBufferUsb), NULL, NULL) == FALSE)
		{
			return GetLastError();
		}

		if (InputBufferUsb[0] == 0x3f)
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