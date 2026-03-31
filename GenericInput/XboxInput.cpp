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
		// NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
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
DWORD XboxInput::SetState(GenericInputController& controller, LPVOID* pData, DWORD dSize)
{
	return ERROR_INVALID_PARAMETER;
}
