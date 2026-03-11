#pragma once
#pragma once
namespace ProController
{
	typedef struct _GENERIC_INPUT_GAMEPAD {
		unsigned short  wButtons;
		unsigned char  bLeftTrigger;
		unsigned char  bRightTrigger;
		short sThumbLX;
		short sThumbLY;
		short sThumbRX;
		short sThumbRY;
	} GENERIC_INPUT_GAMEPAD, * PGENERIC_INPUT_GAMEPAD;

	typedef struct _GENERIC_INPUT {
		unsigned long          dwPacketNumber;
		GENERIC_INPUT_GAMEPAD Gamepad;
	} GENERIC_INPUT_STATE, * PGENERIC_INPUT_STATE;
	enum connectionType
	{
		USB = 513,
		Bluetooth = 362
	};

	DWORD GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState);
	static DWORD SetState(GenericInputController& controller, LPVOID* pData, DWORD dSize);

	static std::vector <BYTE> OutputBuffer;
	static BYTE InputBufferBt[Bluetooth];
	static BYTE InputBufferUsb[USB];

};

