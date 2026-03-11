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

static HMODULE hGenericInput = nullptr;
static HWND hRegisteredWindow = nullptr;
typedef  DWORD(GetState)(DWORD, GENERIC_INPUT_STATE*);
typedef  DWORD(GetLayoutType)(DWORD);

GetState* pGetState = nullptr;
GetLayoutType* pGetLayoutType = nullptr;

// DWORD GenericInputInit(HWND hWindowHandle, BOOL bExitProcess)
// {
// 	hGenericInput = LoadLibraryW(L"GenericInput.dll");
// 	if (hGenericInput == nullptr)
// 	{
// 		MessageBox(hWindowHandle, L"This program can't start because GenericInput.dll is missing from your computer. Try reinstalling the program to fix this problem.", L"System Error", (MB_OK | MB_ICONERROR));
// 	}
// 	else
// 	{
// 		pGetState = (GetState*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(2));
// 		pGetLayoutType = (GetLayoutType*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(9));
// 		if (pGetState == nullptr ||
// 			pGetLayoutType == nullptr)
// 		{
// 			MessageBox(hWindowHandle, L"GenericInput.dll is corrupted. Try reinstalling the program to fix this problem.", L"System Error", (MB_OK | MB_ICONERROR));
// 		}
// 
// 		if (pGetState == nullptr)
// 		{
// 			MessageBox(hWindowHandle, L"GenericInput.dll is corrupted. Try reinstalling the program to fix this problem.", L"System Error", (MB_OK | MB_ICONERROR));
// 		}
// 
// 		if (pGetLayoutType == nullptr)
// 		{
// 			MessageBox(hWindowHandle, L"GenericInput.dll is corrupted. Try reinstalling the program to fix this problem.", L"System Error", (MB_OK | MB_ICONERROR));
// 		}
// 	}
// 
// 
// 	if (bExitProcess == TRUE)
// 	{
// 		ExitProcess(1);
// 	}
// #pragma warning( pop )
// 
// 	DEV_BROADCAST_DEVICEINTERFACE_W db = { 0 };
// 	db.dbcc_size = sizeof(db),
// 		db.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
// 		db.dbcc_classguid = GUID_DEVINTERFACE_XINPUT,
// 		RegisterDeviceNotificationW(hWindowHandle, &db, DEVICE_NOTIFY_WINDOW_HANDLE);
// 
// 	DEV_BROADCAST_DEVICEINTERFACE_W db2 = { 0 };
// 	db2.dbcc_size = sizeof(db2),
// 
// 		db2.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
// 		db2.dbcc_classguid = GUID_DEVINTERFACE_BLUETOOTH,
// 		RegisterDeviceNotificationW(hWindowHandle, &db2, DEVICE_NOTIFY_WINDOW_HANDLE);
// 
// 	DEV_BROADCAST_DEVICEINTERFACE_W db3 = { 0 };
// 	db3.dbcc_size = sizeof(db3),
// 		db3.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
// 		db3.dbcc_classguid = GUID_DEVINTERFACE_HID,
// 		RegisterDeviceNotificationW(hWindowHandle, &db3, DEVICE_NOTIFY_WINDOW_HANDLE);
// 
// 	return ERROR_SUCCESS;
// }
DWORD GenericInputGetState(DWORD dwUserIndex, GENERIC_INPUT_STATE* pState)
{
	if (pState == nullptr || dwUserIndex > 7)
	{
		return ERROR_INVALID_PARAMETER;
	}

	if (hGenericInput == nullptr)
	{
		hGenericInput = LoadLibraryW(L"GenericInput.dll"); // Leaking the library since we want to keep it loaded for the entire process lifetime and we don't have a way to know when to free it
		if (hGenericInput == nullptr)
		{
			return ERROR_FILE_NOT_FOUND;
		}
		if (pGetState == nullptr)
		{
			pGetState = (GetState*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(2));
		}
	}
	if (pGetState)
	{
		return pGetState(dwUserIndex, pState);
	}
	return ERROR_FILE_NOT_FOUND;
}
DWORD GenericInputGetLayout(DWORD dwUserIndex)
{
	if ( dwUserIndex > 7)
	{
		return ERROR_INVALID_PARAMETER;
	}

	if (hGenericInput == nullptr)
	{
		hGenericInput = LoadLibraryW(L"GenericInput.dll"); // Leaking the library since we want to keep it loaded for the entire process lifetime and we don't have a way to know when to free it
		if (hGenericInput == nullptr)
		{
			return ERROR_FILE_NOT_FOUND;
		}
		if (pGetLayoutType == nullptr)
		{
			pGetLayoutType = (GetLayoutType*)GetProcAddress(hGenericInput, MAKEINTRESOURCEA(9));
		}
	}
	if (pGetLayoutType)
	{
		return pGetLayoutType(dwUserIndex);
	}
	return ERROR_FILE_NOT_FOUND;
}
