#include "pch.h"
#include "GenericInput.h"
#include "Window.h"
#include "Bluetooth.h"
#include "ControllerScanner.h"
#include "DualSense.h"
Bluetooth btManager;
Window windowManager;
Scanner controllerScanner;
static GenericInputController ControllerSlots[MAX_CONTROLLERS] = { 0 };
static DWORD LastError;
void GenericInput::Init(HWND hWnd)
{
	if (IsWindow(hWnd) == TRUE)
	{
		if (windowManager.RegisterWindow(hWnd) == FALSE)
		{
			OutputDebugString(L"ERROR: Failed to register window for device notifications.");
		}
	}
	controllerScanner.ScanForControllers(hWnd, ControllerSlots);
}
LRESULT GenericInput::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DEVICECHANGE: {
		switch (wParam)
		{
		case DBT_CUSTOMEVENT:
		{
			DEV_BROADCAST_HDR* dev_broadcast = (DEV_BROADCAST_HDR*)lParam;
			if (dev_broadcast->dbch_devicetype == DBT_DEVTYP_HANDLE)
			{
				DEV_BROADCAST_HANDLE* hdev_broadcast = (DEV_BROADCAST_HANDLE*)lParam;
				if (hdev_broadcast->dbch_eventguid == GUID_BLUETOOTH_L2CAP_EVENT) {

					BTH_L2CAP_EVENT_INFO* eventInfo = (BTH_L2CAP_EVENT_INFO*)hdev_broadcast->dbch_data;
					BLUETOOTH_DEVICE_INFO btDeviceInfo = { 0 };
						for (size_t i = 0; i < MAX_CONTROLLERS; i++)
						{
							if (ControllerSlots[i].ullbtDeviceInfo == eventInfo->bthAddress && eventInfo->connected == 0)
							{
								if (ControllerSlots[i].BTPath != L"")
								{
									if (ControllerSlots[i].Path == L"")
									{
										ControllerSlots[i] = { 0 };
										PostMessage(hWnd, WM_CONTROLLER_DISCONNECTED, i, NULL);
									}
									ControllerSlots[i].BTPath = L"";
								}
								else if (ControllerSlots[i].Path != L"" && ControllerSlots[i].BusType == L"BTHENUM")
								{
									ControllerSlots[i] = { 0 };
									PostMessage(hWnd, WM_CONTROLLER_DISCONNECTED, i, NULL);
								}
								return 0;
							}
							if (ControllerSlots[i].ullbtDeviceInfo == eventInfo->bthAddress && eventInfo->connected == 1 && ControllerSlots[i].BusType == L"BTHENUM")
							{
								return 0;
							}
						}
					if (eventInfo->connected == 1) {
						controllerScanner.ScanForControllers(hWnd, ControllerSlots);
					}
				}
			}
			break;
		}
		case DBT_DEVICEARRIVAL:
		{
			DEV_BROADCAST_HDR* dev_broadcast = (DEV_BROADCAST_HDR*)lParam;
			if (dev_broadcast->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			{
				DEV_BROADCAST_DEVICEINTERFACE* hDevInterface = (DEV_BROADCAST_DEVICEINTERFACE*)lParam;
				if (hDevInterface->dbcc_classguid == GUID_DEVCLASS_XINPUT)
				{
					std::wstring FixedPath = hDevInterface->dbcc_name;
					std::transform(FixedPath.begin(), FixedPath.end(), FixedPath.begin(), ::toupper);
					for (size_t i = 0; i < MAX_CONTROLLERS; i++)
					{
						if (ControllerSlots[i].conType == XInput) {
							std::wstring FixedControllerPath = ControllerSlots[i].XInputPath;
							std::transform(FixedControllerPath.begin(), FixedControllerPath.end(), FixedControllerPath.begin(), ::toupper);
							if (FixedPath == FixedControllerPath)
							{
								return 0;
							}
						}
					}
					controllerScanner.ScanForControllers(hWnd, ControllerSlots);
				}
				else if (hDevInterface->dbcc_classguid == GUID_DEVINTERFACE_HID)
				{
					std::wstring FixedPath = hDevInterface->dbcc_name;
					std::transform(FixedPath.begin(), FixedPath.end(), FixedPath.begin(), ::toupper);
					for (size_t i = 0; i < MAX_CONTROLLERS; i++)
					{
						if (ControllerSlots[i].conType != XInput) {
							std::wstring FixedControllerPath = ControllerSlots[i].Path;
							std::transform(FixedControllerPath.begin(), FixedControllerPath.end(), FixedControllerPath.begin(), ::toupper);
							if (FixedPath == FixedControllerPath)
							{
								return 0;
							}
						}
					}
					controllerScanner.ScanForControllers(hWnd, ControllerSlots);
				}
			}
			break;
		}
		case DBT_DEVICEREMOVECOMPLETE:
		{
			DEV_BROADCAST_HDR* dev_broadcast = (DEV_BROADCAST_HDR*)lParam;
			if (dev_broadcast->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			{
				DEV_BROADCAST_DEVICEINTERFACE* hDevInterface = (DEV_BROADCAST_DEVICEINTERFACE*)lParam;
				if (hDevInterface->dbcc_classguid == GUID_DEVCLASS_XINPUT)
				{
					std::wstring FixedPath = hDevInterface->dbcc_name;
					std::transform(FixedPath.begin(), FixedPath.end(), FixedPath.begin(), ::toupper);
					for (size_t i = 0; i < MAX_CONTROLLERS; i++)
					{
						if (ControllerSlots[i].conType == XInput) {
							std::wstring FixedControllerPath = ControllerSlots[i].XInputPath;
							std::transform(FixedControllerPath.begin(), FixedControllerPath.end(), FixedControllerPath.begin(), ::toupper);
							if (FixedPath == FixedControllerPath)
							{
								ControllerSlots[i] = { 0 };
								PostMessage(hWnd, WM_CONTROLLER_DISCONNECTED, i, NULL);
								return 0;
							}
						}
					}
				}
				else if (hDevInterface->dbcc_classguid == GUID_DEVINTERFACE_HID)
				{
					std::wstring FixedPath = hDevInterface->dbcc_name;
					std::transform(FixedPath.begin(), FixedPath.end(), FixedPath.begin(), ::toupper);
					for (size_t i = 0; i < MAX_CONTROLLERS; i++)
					{
						if (ControllerSlots[i].conType == XInput) {
							std::wstring FixedControllerPath = ControllerSlots[i].Path;
							std::transform(FixedControllerPath.begin(), FixedControllerPath.end(), FixedControllerPath.begin(), ::toupper);
							if (FixedPath == FixedControllerPath && ControllerSlots[i].BTPath == L"")
							{
								ControllerSlots[i] = { 0 };
								PostMessage(hWnd, WM_CONTROLLER_DISCONNECTED, i, NULL);
								return 0;
							}
							else if (FixedPath == FixedControllerPath)
							{
								ControllerSlots[i].Path = L"";
								return 0;
							}
						}
					}
				}
			}
			break;
		}
		}
	}
	}
	return 0;
}
DWORD GenericInput::GetState(DWORD dwUserIndex, GENERIC_INPUT_STATE* pState)
{
	if (pState == nullptr || dwUserIndex > 7)
	{
		return ERROR_INVALID_PARAMETER;
	}
	if (ControllerSlots[dwUserIndex].Path == L"" && ControllerSlots[dwUserIndex].BTPath == L"")
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
	if (ControllerSlots[dwUserIndex].DeviceHandle == 0 && ControllerSlots[dwUserIndex].conType != XInput)
	{
		if (ControllerSlots[dwUserIndex].Path != L"") {
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, NULL, NULL);
		}
		else if (ControllerSlots[dwUserIndex].BTPath != L"")
		{
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].BTPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, NULL, NULL);
		}
		if (ControllerSlots[dwUserIndex].DeviceHandle == INVALID_HANDLE_VALUE || ControllerSlots[dwUserIndex].DeviceHandle == 0)
		{
			return GetLastError();
		}
	}
	switch (ControllerSlots[dwUserIndex].conType)
	{
	case XInput:
	{
		break;
	}
	case DS://DualSense
	{
		return DualSense::GetState(ControllerSlots[dwUserIndex], (DualSense::GENERIC_INPUT_STATE*)pState);
	}
	case SDL:
	{
		break;
	}

	}
	return ERROR_SUCCESS;
}
DWORD GenericInput::GetType(DWORD dwUserIndex)
{
	return ControllerSlots[dwUserIndex].conType;
}