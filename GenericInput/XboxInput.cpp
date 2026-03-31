#include "pch.h"
#include "XboxInput.h"
DWORD XboxInput::GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState)
{
	if (controller == nullptr || controller->DeviceHandle == nullptr || controller->DeviceHandle == INVALID_HANDLE_VALUE || pState == nullptr)
	{
		return ERROR_INVALID_PARAMETER;
	}
	BYTE in[3] = { 0x01, 0x01, 0x00 };
	BYTE out[29] = { 0x00 };
	DWORD size = NULL;
	if (!DeviceIoControl(controller->DeviceHandle, 0x8000e00c, in, sizeof(in), out, sizeof(out), &size, nullptr) || size != sizeof(out))
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
	DWORD size = NULL;
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
