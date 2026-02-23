#pragma once
#include "GenericInputController.h"
#include <xinput.h>
constexpr int WM_CONTROLLER_CONNECTED = 0x8007;
constexpr int WM_CONTROLLER_DISCONNECTED = 0x8008;

constexpr int INPUT_FLAG_GAMEPAD = 0;

constexpr bool bHookWndProc = false;

typedef DWORD(WINAPI* pXInputGetStateEx)(DWORD, XINPUT_STATE*);
typedef DWORD(WINAPI* pXInputWaitForGuideButton)(DWORD, DWORD, LONGLONG);
typedef DWORD(WINAPI* pXInputCancelGuideButtonWait)(DWORD);
typedef DWORD(WINAPI* pXInputPowerOffController)(DWORD);
typedef DWORD(WINAPI* pXInputGetBaseBusInformation)(UINT, LONGLONG, LONGLONG);
typedef DWORD(WINAPI* pXInputGetCapabilitiesEx)(DWORD, LONGLONG, LONGLONG);

typedef DWORD(WINAPI* pXInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*);
typedef void (WINAPI* pXInputEnable)(BOOL);
typedef DWORD(WINAPI* pXInputGetBatteryInformation)(DWORD, BYTE, XINPUT_BATTERY_INFORMATION*);
typedef DWORD(WINAPI* pXInputGetKeystroke)(DWORD, DWORD, PXINPUT_KEYSTROKE);
typedef DWORD(WINAPI* pXInputGetAudioDeviceIds)(DWORD, LPWSTR, UINT*, LPWSTR, UINT*);
typedef DWORD(WINAPI* pXInputGetDSoundAudioDeviceGuids)(DWORD, GUID*, GUID*);

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
    DWORD GetType(DWORD dwUserIndex);
};

namespace XInput
{

};
