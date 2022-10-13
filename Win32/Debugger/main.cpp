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
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
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
		int a = 1;
	}
	return 0;
}