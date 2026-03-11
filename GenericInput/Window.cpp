#include "pch.h"
#include "Window.h"
#include "Bluetooth.h"
#include "controllerDB.h"
extern Bluetooth btManager;
BOOL Window::RegisterWindow(HWND hWindow)
{
	if (IsWindow(hWindow) == FALSE)
	{
		return FALSE;
	}
	if (hDevNotify.size() || btFilters.size())
	{
		UnregisterWindow();
	}
	if (btManager.FindBluetoothRadios(btFilters) == TRUE)
	{
		for (size_t i = 0; i < btFilters.size(); i++)
		{
			HDEVNOTIFY hBTDeviceHID = RegisterDeviceNotification(hWindow, &btFilters[i], DEVICE_NOTIFY_WINDOW_HANDLE);
			if (hBTDeviceHID == 0)
			{
				OutputDebugString(L"Failed to register for bluetooth notifications.");
				break;
			}
			hDevNotify.push_back(hBTDeviceHID);
		}
	}

	DEV_BROADCAST_DEVICEINTERFACE hidFilter = { };
	hidFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	hidFilter.dbcc_classguid = GUID_DEVINTERFACE_HID;
	hidFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	HDEVNOTIFY hDeviceHID = RegisterDeviceNotification(hWindow, &hidFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (hDeviceHID == 0)
	{
		OutputDebugString(L"Failed to register for hid notifications.");
		return FALSE;
	}
	hDevNotify.push_back(hDeviceHID);

	DEV_BROADCAST_DEVICEINTERFACE xinputFilter = { };
	xinputFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	xinputFilter.dbcc_classguid = GUID_DEVCLASS_XINPUT;
	xinputFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	HDEVNOTIFY hDeviceXinputHID = RegisterDeviceNotification(hWindow, &xinputFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

	if (hDeviceXinputHID == 0)
	{
		OutputDebugString(L"Failed to register for xinput notifications.");
		return FALSE;
	}
	hDevNotify.push_back(hDeviceXinputHID);

	return TRUE;
}
void Window::UnregisterWindow()
{
	for (size_t i = 0; i < hDevNotify.size(); i++)
	{
		if (hDevNotify[i] != INVALID_HANDLE_VALUE)
		{
			UnregisterDeviceNotification(hDevNotify[i]);
		}
	}
	for (size_t i = 0; i < btFilters.size(); i++)
	{
		if (btFilters[i].dbch_handle != 0)
		{
			CloseHandle(btFilters[i].dbch_handle);
		}
	}
	hDevNotify.clear();
	btFilters.clear();
}
/*BOOL Window::isController(std::wstring& DevicePath)
{
	HANDLE deviceHandle = CreateFile(DevicePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (deviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	HIDD_ATTRIBUTES hidArtib = { };
	if (HidD_GetAttributes(deviceHandle, &hidArtib) == FALSE)
	{
		CloseHandle(deviceHandle);
		return FALSE;
	}
	CloseHandle(deviceHandle);
	for (size_t i = 0; i < _countof(gamepadDB); i++)
	{
		if (hidArtib.VendorID == gamepadDB[i].VID && hidArtib.ProductID == gamepadDB[i].PID)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Window::isXinput(std::wstring& DevicePath)
{
	HANDLE deviceHandle = CreateFile(DevicePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (deviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	BYTE in[3] = { 0x01, 0x01, 0x00 };
	BYTE out[4];
	DWORD size;
	if (!DeviceIoControl(deviceHandle, 0x8000e00c, in, sizeof(in), out, sizeof(out), &size, NULL) || size != sizeof(out))
	{
		// NOTE: could check GetLastError() here, if it is ERROR_DEVICE_NOT_CONNECTED - that means disconnect
		DWORD er = GetLastError();
		return FALSE;
	}
	return TRUE;
}
*/