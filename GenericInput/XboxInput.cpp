#include "pch.h"
#include "XboxInput.h"
DWORD dwDeviceIOCodeXbox = 0x8000e00c;
DWORD XboxInput::GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState)
{
	if (controller == nullptr || controller->DeviceHandle == nullptr || controller->DeviceHandle == INVALID_HANDLE_VALUE || pState == nullptr)
	{
		return ERROR_INVALID_PARAMETER;
	}
	BYTE in[3] = { 0x01, 0x01, 0x00 };
	BYTE out[29] = { 0x00 };
	DWORD size = NULL;
	if (!DeviceIoControl(controller->DeviceHandle, dwDeviceIOCodeXbox, in, sizeof(in), out, sizeof(out), &size, nullptr) || size != sizeof(out))
	{
		if (GetLastError() == ERROR_DEVICE_NOT_CONNECTED )
		{
			CloseHandle(controller->DeviceHandle);
			controller = nullptr;
			return ERROR_DEVICE_NOT_CONNECTED;
		}
	}

	pState->dwPacketNumber = *(DWORD*)(out + 5);
	pState->Gamepad.wButtons = *(WORD*)(out + 11);
	pState->Gamepad.bLeftTrigger = out[13];
	pState->Gamepad.bRightTrigger = out[14];
	pState->Gamepad.sThumbLX = *(SHORT*)(out + 15);
	pState->Gamepad.sThumbLY = *(SHORT*)(out + 17);
	pState->Gamepad.sThumbRX = *(SHORT*)(out + 19);
	pState->Gamepad.sThumbRY = *(SHORT*)(out + 21);
	return ERROR_SUCCESS;
}
DWORD XboxInput::SetState(GenericInputController* controller, GENERIC_VIBRATION* pVibration)
{
	if (controller == nullptr || controller->DeviceHandle == nullptr || controller->DeviceHandle == INVALID_HANDLE_VALUE || pVibration == nullptr)
	{
		return ERROR_INVALID_PARAMETER;
	}

	BYTE high_Freq = static_cast<unsigned char>(pVibration->wRightMotorSpeed);
	BYTE low_Freq = static_cast<unsigned char>(pVibration->wLeftMotorSpeed);

	BYTE in[5] = { 0x00, 0x00, low_Freq,  high_Freq, 0x02 };
	if (!DeviceIoControl(controller->DeviceHandle, 0x8000a010, in, sizeof(in), nullptr, 0, nullptr, nullptr))
	{
		if (GetLastError() == ERROR_DEVICE_NOT_CONNECTED)
		{
			CloseHandle(controller->DeviceHandle);
			controller = nullptr;
			return ERROR_DEVICE_NOT_CONNECTED;
		}
	}
	return ERROR_SUCCESS;
}
DWORD  XboxInput::GetCapabilities(GenericInputController* controller, GENERIC_CAPABILITIES* pCapabilities)
{
	if (controller == nullptr || controller->DeviceHandle == nullptr || controller->DeviceHandle == INVALID_HANDLE_VALUE || pCapabilities == nullptr)
	{
		return ERROR_INVALID_PARAMETER;
	}

	BYTE in[3] = { 0x01, 0x01, 0x00 };
	BYTE out[24];
	DWORD size = NULL;
	if (!DeviceIoControl(controller->DeviceHandle, 0x8000e004, in, sizeof(in), out, sizeof(out), &size, nullptr) || size != sizeof(out))
	{
		// NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
		return ERROR_DEVICE_NOT_CONNECTED;
	}

	pCapabilities->Type = out[2];
	pCapabilities->SubType = out[3];
	pCapabilities->Flags = 4; // yes, always 4
	pCapabilities->Gamepad.wButtons = *(WORD*)(out + 4);
	pCapabilities->Gamepad.bLeftTrigger = out[6];
	pCapabilities->Gamepad.bRightTrigger = out[7];
	pCapabilities->Gamepad.sThumbLX = *(SHORT*)(out + 8);
	pCapabilities->Gamepad.sThumbLY = *(SHORT*)(out + 10);
	pCapabilities->Gamepad.sThumbRX = *(SHORT*)(out + 12);
	pCapabilities->Gamepad.sThumbRY = *(SHORT*)(out + 14);
	pCapabilities->Vibration.wLeftMotorSpeed = out[22];
	pCapabilities->Vibration.wRightMotorSpeed = out[23];
	return ERROR_SUCCESS;
}