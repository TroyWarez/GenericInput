// Generic Input.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include <dbt.h>
#include <setupapi.h>
#pragma warning( push )
#pragma warning( disable : 4191)
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

HMODULE hGenericInput = nullptr;
typedef  LRESULT(Proc)(HWND, UINT, WPARAM, LPARAM);
typedef  DWORD(GetState)(DWORD, GENERIC_INPUT_STATE*);
typedef  DWORD(GetType)(DWORD);

Proc* pProc = nullptr;
GetState* pGetState = nullptr;
GetType* pGetType = nullptr;
DWORD GenericInputInit(HWND hWindowHandle, BOOL bExitProcess)
{
	if (bExitProcess == TRUE)
	{
		hGenericInput = LoadLibrary(L"GenericInput.dll");
		if (hGenericInput == NULL)
		{
			MessageBox(hWindowHandle, L"This program can't start because GenericInput.dll is missing from your computer. Try reinstalling the program to fix this problem.", L"System Error", (MB_OK | MB_ICONERROR));
			ExitProcess(1);
		}
		pProc = (Proc*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(31));
		if (pProc == nullptr)
		{
			MessageBox(hWindowHandle, L"GenericInput.dll is corrupted. Try reinstalling the program to fix this problem.", L"System Error", (MB_OK | MB_ICONERROR));
			ExitProcess(1);
		}
		pGetState = (GetState*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(4));
		if (pGetState == nullptr)
		{
			MessageBox(hWindowHandle, L"GenericInput.dll is corrupted. Try reinstalling the program to fix this problem.", L"System Error", (MB_OK | MB_ICONERROR));
			ExitProcess(1);
		}
		pGetType = (GetType*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(32));
		if (pGetType == nullptr)
		{
			MessageBox(hWindowHandle, L"GenericInput.dll is corrupted. Try reinstalling the program to fix this problem.", L"System Error", (MB_OK | MB_ICONERROR));
			ExitProcess(1);
		}
	}
	else
	{
		hGenericInput = LoadLibrary(L"GenericInput.dll");
		if (hGenericInput == NULL)
		{
			return ERROR_FILE_NOT_FOUND;
		}
		pProc = (Proc*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(31));
		if (pProc == nullptr)
		{
			return ERROR_FILE_NOT_FOUND;
		}
		pGetState = (GetState*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(4));
		if (pGetState == nullptr)
		{
			return ERROR_FILE_NOT_FOUND;
		}
		pGetType = (GetType*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(32));
		if (pGetType == nullptr)
		{
			return ERROR_FILE_NOT_FOUND;
		}
	}
#pragma warning( pop )

	DEV_BROADCAST_DEVICEINTERFACE_W db = { 0 };
	db.dbcc_size = sizeof(db),
		db.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
		db.dbcc_classguid = GUID_DEVINTERFACE_XINPUT,
		RegisterDeviceNotificationW(hWindowHandle, &db, DEVICE_NOTIFY_WINDOW_HANDLE);

	DEV_BROADCAST_DEVICEINTERFACE_W db2 = { 0 };
	db2.dbcc_size = sizeof(db2),

		db2.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
		db2.dbcc_classguid = GUID_DEVINTERFACE_BLUETOOTH,
		RegisterDeviceNotificationW(hWindowHandle, &db2, DEVICE_NOTIFY_WINDOW_HANDLE);

	DEV_BROADCAST_DEVICEINTERFACE_W db3 = { 0 };
	db3.dbcc_size = sizeof(db3),
		db3.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
		db3.dbcc_classguid = GUID_DEVINTERFACE_HID,
		RegisterDeviceNotificationW(hWindowHandle, &db3, DEVICE_NOTIFY_WINDOW_HANDLE);

	return ERROR_SUCCESS;
}
LRESULT GenericInputDeviceChange(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (pProc)
	{
		return pProc(hWnd, message, wParam, lParam);
	}
	return ERROR_FILE_NOT_FOUND;
}
DWORD GenericInputGetState(DWORD dwUserIndex, GENERIC_INPUT_STATE* pState)
{
	if (pGetState)
	{
		return pGetState(dwUserIndex, pState);
	}
	return ERROR_FILE_NOT_FOUND;
}
DWORD GenericInputGetLayout(DWORD dwUserIndex)
{
	if (pGetType)
	{
		return pGetType(dwUserIndex);
	}
	return ERROR_FILE_NOT_FOUND;
}
