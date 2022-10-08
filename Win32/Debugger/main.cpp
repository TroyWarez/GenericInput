#include <Windows.h>
#include <iostream>
#include "GenericInput.h"
// Structures used by XInput APIs
//
typedef struct _XINPUT_GAMEPAD
{
	WORD                                wButtons;
	BYTE                                bLeftTrigger;
	BYTE                                bRightTrigger;
	SHORT                               sThumbLX;
	SHORT                               sThumbLY;
	SHORT                               sThumbRX;
	SHORT                               sThumbRY;
} XINPUT_GAMEPAD, * PXINPUT_GAMEPAD;

typedef struct _XINPUT_STATE
{
	DWORD                               dwPacketNumber;
	XINPUT_GAMEPAD                      Gamepad;
} XINPUT_STATE, * PXINPUT_STATE;

typedef DWORD WINAPI XInputGetState
(
	_In_  DWORD         dwUserIndex,  // Index of the gamer associated with the device
	_Out_ XINPUT_STATE* pState        // Receives the current state
);
int main(void)
{
	HMODULE hGenericInputLib = LoadLibrary(L"GenericInput.dll");
	XINPUT_STATE xState = { 0 };
	if (hGenericInputLib == NULL)
	{
		MessageBox(NULL, L"\"GenericInput.dll\" was not found.", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	XInputGetState* pGetStateProc = (XInputGetState*)GetProcAddress(hGenericInputLib, "XInputGetState");// Unsafe
	while (IsDebuggerPresent() == TRUE)
	{
		DWORD Error = (DWORD)(XInputGetState*)pGetStateProc(0, &xState);
		switch (Error)
		{
		case ERROR_SUCCESS:
		{
			OutputDebugStringW(L"Controller connected.\n");
			switch (xState.Gamepad.wButtons)
			{
			case CONTROLLER_BUTTON_A:
			{
				OutputDebugString(L"Button Pressed: \"A\"");
			}
			case CONTROLLER_BUTTON_B:
			{
				OutputDebugString(L"Button Pressed: \"B\"");
			}
			case CONTROLLER_BUTTON_X:
			{
				OutputDebugString(L"Button Pressed: \"X\"");
			}
			case CONTROLLER_BUTTON_Y:
			{
				OutputDebugString(L"Button Pressed: \"Y\"");
			}
			case CONTROLLER_BUTTON_BACK:
			{
				OutputDebugString(L"Button Pressed: \"BACK\"");
			}
			case CONTROLLER_BUTTON_GUIDE:
			{
				OutputDebugString(L"Button Pressed: \"GUIDE\"");
			}
			case CONTROLLER_BUTTON_START:
			{
				OutputDebugString(L"Button Pressed: \"START\"");
			}
			case CONTROLLER_BUTTON_LEFTSTICK:
			{
				OutputDebugString(L"Button Pressed: \"LEFTSTICK\"");
			}
			case CONTROLLER_BUTTON_RIGHTSTICK:
			{
				OutputDebugString(L"Button Pressed: \"RIGHTSTICK\"");
			}
			case CONTROLLER_BUTTON_LEFTSHOULDER:
			{
				OutputDebugString(L"Button Pressed: \"LEFTSHOULDER\"");
			}
			case CONTROLLER_BUTTON_RIGHTSHOULDER:
			{
				OutputDebugString(L"Button Pressed: \"RIGHTSHOULDER\"");
			}
			case CONTROLLER_BUTTON_DPAD_UP:
			{
				OutputDebugString(L"Button Pressed: \"DPAD UP\"");
			}
			case CONTROLLER_BUTTON_DPAD_DOWN:
			{
				OutputDebugString(L"Button Pressed: \"DPAD DOWN\"");
			}
			case CONTROLLER_BUTTON_DPAD_LEFT:
			{
				OutputDebugString(L"Button Pressed: \"DPAD LEFT\"");
			}
			case CONTROLLER_BUTTON_DPAD_RIGHT:
			{
				OutputDebugString(L"Button Pressed: \"DPAD RIGHT\"");
			}
			}
		}
		default:
		{
			OutputDebugStringW(L"Unknown Error occured\n");
		}
		}

	}
	return 0;
}