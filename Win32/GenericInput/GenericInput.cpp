#include "pch.h"
#include "GenericInput.h"
#include "Window.h"
#include "Bluetooth.h"
#include "ControllerScanner.h"
#include "DualSense.h"
#include "DualShock4.h"
#include "ProController.h"
Bluetooth btManager;
Window windowManager;
Scanner controllerScanner;
extern HMODULE g_xinputModule;
static GenericInputController ControllerSlots[MAX_CONTROLLERS];
static DWORD LastError;
static bool RegisterWindowFlag = false;
static BOOL CALLBACK GenericInput::EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
	DWORD lpdwProcessId = 0;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == lParam)
	{
		controllerScanner.ScanForControllers(hwnd, ControllerSlots);
		windowManager.RegisterWindow(hwnd);
		RegisterWindowFlag = true;
		return FALSE;
	}
	return TRUE;
}

LRESULT GenericInput::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!RegisterWindowFlag)
	{
		windowManager.RegisterWindow(hWnd);
		controllerScanner.ScanForControllers(hWnd, ControllerSlots);
		RegisterWindowFlag = true;
	}
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
DWORD GenericInput::XInputGetState(DWORD dwUserIndex, GENERIC_INPUT_STATE* pState)
{
	if (pState == nullptr || dwUserIndex > 7)
	{
		return ERROR_INVALID_PARAMETER;
	}
	if (ControllerSlots[dwUserIndex].Path == L"" && ControllerSlots[dwUserIndex].BTPath == L"")
	{
		if (!RegisterWindowFlag)
		{
			DWORD currentPid = GetProcessId(GetCurrentProcess());
			EnumWindows(EnumWindowsProc, currentPid);
		}
		else
		{
			return ERROR_DEVICE_NOT_CONNECTED;
		}
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
		//return GameInput::GetState(ControllerSlots[dwUserIndex], pState);
	}
	case DS://DualSense
	{
		return DualSense::GetState(&ControllerSlots[dwUserIndex], (DualSense::GENERIC_INPUT_STATE*)pState);
	}
	case DS4://DualSense
	{
		return DualShock4::GetState(&ControllerSlots[dwUserIndex], (DualShock4::GENERIC_INPUT_STATE*)pState);
	}
	case NT://Pro controller
	{
		return ProController::GetState(&ControllerSlots[dwUserIndex], (ProController::GENERIC_INPUT_STATE*)pState);
	}
	case SDL:
	{
		break;
	}

	}
	return ERROR_SUCCESS;
}
DWORD GenericInput::XInputSetState(DWORD dwUserIndex, INPUT_VIBRATION* pVibration)
{
	if (pVibration == nullptr || dwUserIndex > 7)
	{
		return ERROR_INVALID_PARAMETER;
	}
	return ERROR_SUCCESS;
}
// DWORD XInput1_3::XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)//Ignore the directsound stuff, the guids are for modern sound devices...
// {
// 	/*To support this I will need to add methods to get audio from controllers and microphone support*/
// 	return ERROR_DEVICE_NOT_CONNECTED;
// }
// DWORD WINAPI XInput1_3::XInputGetCapabilities
// (
// 	_In_  DWORD                dwUserIndex,   // Index of the gamer associated with the device
// 	_In_  DWORD                dwFlags,       // Input flags that identify the device type
// 	_Out_ XINPUT_CAPABILITIES* pCapabilities  // Receives the capabilities
// )
// {
// 	switch (dwFlags)
// 	{
// 	case INPUT_FLAG_GAMEPAD:
// 	{
// 		if (ControllerSlots[dwUserIndex].conType == controllerType::XInput)
// 		{
// 			return ERROR_SUCCESS;
// 		}
// 		break;
// 	}
// 	default:
// 	{
// 		return ERROR_DEVICE_NOT_CONNECTED;// Non gamepads aren't supported yet.
// 	}
// 	}
// 
// 	return ERROR_DEVICE_NOT_CONNECTED;
// }
// DWORD GenericInput::XInputEnable(BOOL enable)
// {
// 	return ERROR_SUCCESS;
// }
DWORD GenericInput::GetType(DWORD dwUserIndex)
{
	return ControllerSlots[dwUserIndex].conType;
}