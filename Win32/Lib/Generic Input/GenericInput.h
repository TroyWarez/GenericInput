#pragma once
#include "pch.h"

#define WM_CONTROLLER_CONNECTED 0x8007
#define WM_CONTROLLER_DISCONNECTED 0x8008

#define CONTROLLER_BUTTON_A 0x1000
#define CONTROLLER_BUTTON_B 0x2000
#define CONTROLLER_BUTTON_X 0x4000
#define CONTROLLER_BUTTON_Y 0x8000
#define CONTROLLER_BUTTON_BACK 0x0020
#define CONTROLLER_GAMEPAD_GUIDE 0x0400
#define CONTROLLER_BUTTON_START 0x0010
#define CONTROLLER_BUTTON_LEFTSTICK 0x0040
#define CONTROLLER_BUTTON_RIGHTSTICK 0x0080
#define CONTROLLER_BUTTON_LEFTSHOULDER 0x0100
#define CONTROLLER_BUTTON_RIGHTSHOULDER 0x0200
#define CONTROLLER_BUTTON_DPAD_UP 0x0001
#define CONTROLLER_BUTTON_DPAD_DOWN 0x0002
#define CONTROLLER_BUTTON_DPAD_LEFT 0x0004
#define CONTROLLER_BUTTON_DPAD_RIGHT 0x0008

#define CONTROLLER_GAMEPAD_PADDLE_1 0x0005
#define CONTROLLER_GAMEPAD_PADDLE_2 0x0006
#define CONTROLLER_GAMEPAD_PADDLE_3 0x0007
#define CONTROLLER_GAMEPAD_PADDLE_4 0x0008


enum controllerType
{
	NC = 0,
	XInput = 1,
	DS = 2,
    NT = 3,
	SDL = 4
};
typedef struct _GENERIC_INPUT_GAMEPAD
{
    WORD                                wButtons;
    BYTE                                bLeftTrigger;
    BYTE                                bRightTrigger;
    SHORT                               sThumbLX;
    SHORT                               sThumbLY;
    SHORT                               sThumbRX;
    SHORT                               sThumbRY;
} GENERIC_INPUT_GAMEPAD, * PGENERIC_INPUT_GAMEPAD;

typedef struct _GENERIC_INPUT
{
    DWORD                               dwPacketNumber;
    GENERIC_INPUT_GAMEPAD               Gamepad;
}  GENERIC_INPUT_STATE, * PGENERIC_INPUT_STATE;

DWORD GenericInputInit(HWND hWindowHandle, BOOL bExitProcess);
LRESULT GenericInputDeviceChange(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD GenericInputGetState(DWORD dwUserIndex, GENERIC_INPUT_STATE* pState);
DWORD GenericInputGetLayout(DWORD dwUserIndex);