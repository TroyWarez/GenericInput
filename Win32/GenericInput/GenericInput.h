#pragma once
#include "GenericInputController.h"
constexpr int WM_CONTROLLER_CONNECTED = 0x8007;
constexpr int WM_CONTROLLER_DISCONNECTED = 0x8008;

constexpr int INPUT_FLAG_GAMEPAD = 0;

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

typedef struct _GENERIC_VIBRATION
{
    WORD                                wLeftMotorSpeed;
    WORD                                wRightMotorSpeed;
} GENERIC_VIBRATION, * PGENERIC_VIBRATION;

typedef struct _GENERIC_CAPABILITIES {
    BYTE             Type;
    BYTE             SubType;
    WORD             Flags;
    GENERIC_INPUT_GAMEPAD   Gamepad;
    GENERIC_VIBRATION Vibration;
} GENERIC_CAPABILITIES, * PGENERIC_CAPABILITIES;

typedef struct _INPUT_VIBRATION
{
    WORD                                wLeftMotorSpeed;
    WORD                                wRightMotorSpeed;
} INPUT_VIBRATION, * PINPUT_VIBRATION;

namespace GenericInput
{
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    DWORD XInputGetState(DWORD dwUserIndex, GENERIC_INPUT_STATE* pState);
    DWORD XInputSetState(DWORD dwUserIndex, INPUT_VIBRATION* pVibration);

	BOOL CALLBACK EnumWindowsProc(
		_In_ HWND   hwnd,
		_In_ LPARAM lParam
	);

    DWORD XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);

    DWORD XInputGetCapabilities(DWORD dwUserIndex, DWORD wFlags, GENERIC_CAPABILITIES* pCapabilities);
    DWORD GetType(DWORD dwUserIndex);

};
