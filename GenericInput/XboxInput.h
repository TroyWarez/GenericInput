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

	DWORD GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState);
	static DWORD SetState(GenericInputController& controller, LPVOID* pData, DWORD dSize);

	static std::vector <BYTE> OutputBuffer;

};

