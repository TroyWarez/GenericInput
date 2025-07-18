#pragma once
namespace DualShock4
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
		USB_Dongle = 64,
		Bluetooth = 547
	};

	DWORD GetState(GenericInputController* controller, GENERIC_INPUT_STATE* pState);
	BOOL IsDualshock4Connected(std::wstring& DevicePath);
	const USHORT DualShock4DongleVID = 0x054c;
	const USHORT DualShock4DonglePID = 0x0BA0;
	const USHORT DualShock4DongleDFUPID = 0x0BA1;
	static DWORD SetState(GenericInputController& controller, LPVOID* pData, DWORD dSize);

	static std::vector <BYTE> OutputBuffer;
	static BYTE InputBufferBt[Bluetooth];
	static BYTE InputBufferUsb[USB_Dongle];

};

