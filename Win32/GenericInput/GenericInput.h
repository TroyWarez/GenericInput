#pragma once
#include "GenericInputController.h"
constexpr int WM_CONTROLLER_CONNECTED = 0x8007;
constexpr int WM_CONTROLLER_DISCONNECTED = 0x8008;

constexpr int INPUT_FLAG_GAMEPAD = 0;

constexpr bool bHookWndProc = false;

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

typedef struct _GENERIC_BATTERY_INFORMATION
{
	BYTE BatteryType;
	BYTE BatteryLevel;
} GENERIC_BATTERY_INFORMATION, * PGENERIC_BATTERY_INFORMATION;

typedef struct _GENERIC_KEYSTROKE
{
	WORD    VirtualKey;
	WCHAR   Unicode;
	WORD    Flags;
	BYTE    UserIndex;
	BYTE    HidCode;
} GENERIC_KEYSTROKE, * PGENERIC_KEYSTROKE;

typedef DWORD(WINAPI* pXInputGetStateEx)(DWORD, PGENERIC_INPUT_STATE);
typedef DWORD(WINAPI* pXInputWaitForGuideButton)(DWORD, DWORD, PGENERIC_INPUT_STATE);
typedef DWORD(WINAPI* pXInputCancelGuideButtonWait)(DWORD);
typedef DWORD(WINAPI* pXInputPowerOffController)(DWORD);
typedef DWORD(WINAPI* pXInputGetBaseBusInformation)(UINT, PGENERIC_CAPABILITIES, PGENERIC_CAPABILITIES);
typedef DWORD(WINAPI* pXInputGetCapabilitiesEx)(DWORD, PGENERIC_CAPABILITIES, PGENERIC_CAPABILITIES);

typedef DWORD(WINAPI* pXInputGetCapabilities)(DWORD, DWORD, PGENERIC_CAPABILITIES);
typedef void (WINAPI* pXInputEnable)(BOOL);
typedef DWORD(WINAPI* pXInputGetBatteryInformation)(DWORD, BYTE, PGENERIC_BATTERY_INFORMATION);
typedef DWORD(WINAPI* pXInputGetKeystroke)(DWORD, DWORD, PGENERIC_KEYSTROKE);
typedef DWORD(WINAPI* pXInputGetAudioDeviceIds)(DWORD, LPWSTR, UINT*, LPWSTR, UINT*);
typedef DWORD(WINAPI* pXInputGetDSoundAudioDeviceGuids)(DWORD, GUID*, GUID*);

namespace GenericInput
{
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    DWORD XInputGetState(DWORD dwUserIndex, PGENERIC_INPUT_STATE pState);
    DWORD XInputSetState(DWORD dwUserIndex, PGENERIC_VIBRATION pVibration);

	BOOL CALLBACK EnumWindowsProc(
		_In_ HWND   hwnd,
		_In_ LPARAM lParam
	);
    DWORD GetType(DWORD dwUserIndex);
};

namespace XInputDLL
{
	DWORD XInputGetStateEx (DWORD dwUserIndex, PGENERIC_INPUT_STATE pState);
	DWORD XInputWaitForGuideButton (DWORD dwUserIndex, DWORD dwFlags, PGENERIC_INPUT_STATE pState); // Broken
	DWORD XInputCancelGuideButtonWait (DWORD dwUserIndex);
	DWORD XInputPowerOffController (DWORD dwUserIndex);
	DWORD XInputGetBaseBusInformation (DWORD dwUserIndex, PGENERIC_CAPABILITIES pCapabilities, PGENERIC_CAPABILITIES pCapabilities2); // Broken
	DWORD XInputGetCapabilitiesEx (DWORD dwUserIndex, PGENERIC_CAPABILITIES pCapabilities, PGENERIC_CAPABILITIES pCapabilities2); // Broken

	DWORD XInputGetCapabilities (DWORD dwUserIndex, DWORD dwFlags, PGENERIC_CAPABILITIES pCapabilities);
	void  XInputEnable (BOOL enable);
	DWORD XInputGetBatteryInformation (DWORD dwUserIndex, BYTE devType, PGENERIC_BATTERY_INFORMATION pBatteryInformation);
	DWORD XInputGetKeystroke (DWORD dwUserIndex, DWORD dwReserved, PGENERIC_KEYSTROKE pKeystroke);
	DWORD XInputGetAudioDeviceIds (DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount);
	DWORD XInputGetDSoundAudioDeviceGuids (DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
};
