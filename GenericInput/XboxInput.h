#pragma once
namespace XboxInput
{
	typedef struct _GENERIC_INPUT_GAMEPAD {
		WORD                                wButtons;
		BYTE                                bLeftTrigger;
		BYTE                                bRightTrigger;
		SHORT                               sThumbLX;
		SHORT                               sThumbLY;
		SHORT                               sThumbRX;
		SHORT                               sThumbRY;
	} GENERIC_INPUT_GAMEPAD, * PGENERIC_INPUT_GAMEPAD;

	typedef struct _GENERIC_INPUT {
		DWORD          dwPacketNumber;
		GENERIC_INPUT_GAMEPAD Gamepad;
	} GENERIC_INPUT_STATE, * PGENERIC_INPUT_STATE;
	typedef struct _GENERIC_VIBRATION
	{
		WORD                                wLeftMotorSpeed;
		WORD                                wRightMotorSpeed;
	} GENERIC_VIBRATION, * PGENERIC_VIBRATION;
	typedef struct _XINPUT_CAPABILITIES
	{
		BYTE                                Type;
		BYTE                                SubType;
		WORD                                Flags;
		GENERIC_INPUT_GAMEPAD                      Gamepad;
		GENERIC_VIBRATION                    Vibration;
	} GENERIC_CAPABILITIES, * PGENERIC_CAPABILITIES;

	DWORD GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState);
	DWORD SetState(GenericInputController* controller, GENERIC_VIBRATION* pVibration);
	DWORD GetCapabilities(GenericInputController* controller, GENERIC_CAPABILITIES* pCapabilities);
	static std::vector <BYTE> OutputBuffer;

};

