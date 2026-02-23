#pragma once
#include "GenericInputController.h"
#include <xinput.h>
constexpr int WM_CONTROLLER_CONNECTED = 0x8007;
constexpr int WM_CONTROLLER_DISCONNECTED = 0x8008;

constexpr int INPUT_FLAG_GAMEPAD = 0;

constexpr bool bHookWndProc = false;

typedef DWORD pXInputGetStateEx (DWORD, PXINPUT_STATE);
typedef DWORD pXInputWaitForGuideButton (DWORD, DWORD, LONGLONG);
typedef DWORD pXInputCancelGuideButtonWait (DWORD);
typedef DWORD pXInputPowerOffController (DWORD);
typedef DWORD pXInputGetBaseBusInformation (DWORD, LONGLONG, LONGLONG);
typedef DWORD pXInputGetCapabilitiesEx (DWORD, LONGLONG, LONGLONG);

typedef DWORD pXInputGetCapabilities (DWORD, DWORD, PXINPUT_CAPABILITIES);
typedef void  pXInputEnable (BOOL);
typedef DWORD pXInputGetBatteryInformation (DWORD, BYTE, PXINPUT_BATTERY_INFORMATION);
typedef DWORD pXInputGetKeystroke (DWORD, DWORD, PXINPUT_KEYSTROKE);
typedef DWORD pXInputGetAudioDeviceIds (DWORD, LPWSTR, UINT*, LPWSTR, UINT*);
typedef DWORD pXInputGetDSoundAudioDeviceGuids (DWORD, GUID*, GUID*);

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

namespace XInputExports
{
	DWORD XInputGetStateEx (DWORD dwUserIndex, XINPUT_STATE* pState);
	DWORD XInputWaitForGuideButton (DWORD dwUserIndex, DWORD dwFlag, LONGLONG lUnkown);
	DWORD XInputCancelGuideButtonWait (DWORD dwUserIndex);
	DWORD XInputPowerOffController (DWORD dwUserIndex);
	DWORD XInputGetBaseBusInformation (DWORD dwUserIndex, LONGLONG lUnkown, LONGLONG lUnkown2);
	DWORD XInputGetCapabilitiesEx (DWORD dwUserIndex, LONGLONG lUnkown, LONGLONG lUnkown2);

	DWORD XInputGetCapabilities (DWORD dwUserIndex, DWORD dwFlags, PXINPUT_CAPABILITIES pState);
	void  XInputEnable (BOOL enable);
	DWORD XInputGetBatteryInformation (DWORD dwUserIndex, BYTE devType, PXINPUT_BATTERY_INFORMATION pBatteryInformation);
	DWORD XInputGetKeystroke (DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke);
	DWORD XInputGetAudioDeviceIds (DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount);
	DWORD XInputGetDSoundAudioDeviceGuids (DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
};
