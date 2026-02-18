#pragma once
#include "GenericInputController.h"
#include <xinput.h>
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
    DWORD GetType(DWORD dwUserIndex);
};

namespace XInput1_3
{
	typedef DWORD(WINAPI* PFN_XInputGetCapabilities)(
		_In_  DWORD                dwUserIndex,
		_In_  DWORD                dwFlags,
		_Out_ XINPUT_CAPABILITIES* pCapabilities
		);
	typedef DWORD WINAPI PFN_XInputGetDSoundAudioDeviceGuids
	(
		_In_  DWORD     dwUserIndex,          // Index of the gamer associated with the device
		_Out_ GUID* pDSoundRenderGuid,    // DSound device ID for render (speakers)
		_Out_ GUID* pDSoundCaptureGuid    // DSound device ID for capture (microphone)
	);

	typedef void WINAPI PFN_XInputEnable
	(
		_In_ BOOL enable     // [in] Indicates whether xinput is enabled or disabled. 
	) WIN_NOEXCEPT;

	typedef DWORD WINAPI PFN_XInputGetCapabilities
	(
		_In_  DWORD                dwUserIndex,   // Index of the gamer associated with the device
		_In_  DWORD                dwFlags,       // Input flags that identify the device type
		_Out_ XINPUT_CAPABILITIES* pCapabilities  // Receives the capabilities
	) WIN_NOEXCEPT;

};
