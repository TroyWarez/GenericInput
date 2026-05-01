#include "pch.h"
#include "GenericInput.h"
#include "Window.h"
#include "Bluetooth.h"
#include "ControllerScanner.h"
#include "DualSense.h"
#include "DualShock4.h"
#include "ProController.h"
#include "XboxInput.h"
Bluetooth btManager;
Window windowManager;
Scanner controllerScanner;
extern HMODULE g_xinputModule;

// Ordinals
extern pXInputGetStateEx funcGetStateEx;
extern pXInputWaitForGuideButton  funcWaitForGuideButton;
extern pXInputCancelGuideButtonWait  funcCancelGuideButtonWait;
extern pXInputPowerOffController  funcPowerOffController;
extern pXInputGetBaseBusInformation  funcBaseBusInformation;
extern pXInputGetCapabilitiesEx funcGetCapabilitiesEx;

extern pXInputGetCapabilities funcGetCapabilities;
extern pXInputEnable funcEnable;
extern pXInputGetBatteryInformation funcGetBatteryInformation;
extern pXInputGetKeystroke funcGetKeystroke;
extern pXInputGetAudioDeviceIds funcGetAudioDeviceIds;
extern pXInputGetDSoundAudioDeviceGuids funcGetDSoundGuids;

extern bool RegisterWindowFlag;

static DWORD LastError = NULL;
static bool RegisterWindowFlag = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
							if (ControllerSlots[i].BTPath.empty())
							{
								if (ControllerSlots[i].Path.empty())
								{
									ControllerSlots[i] = { 0 };
									PostMessage(hWnd, WM_CONTROLLER_DISCONNECTED, i, NULL);
								}
								ControllerSlots[i].BTPath = L"";
							}
							else if (ControllerSlots[i].Path.empty() && ControllerSlots[i].BusType == L"BTHENUM")
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
							if (FixedPath == FixedControllerPath && ControllerSlots[i].BTPath.empty())
							{
								ControllerSlots[i] = { 0 };
								PostMessage(hWnd, WM_CONTROLLER_DISCONNECTED, i, NULL);
								return 0;
							}
							if (FixedPath == FixedControllerPath)
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
		break;
	}
	case WM_DESTROY:
	{
		windowManager.UnregisterWindow();
		RegisterWindowFlag = false;
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
DWORD GenericInput::XInputGetState(DWORD dwUserIndex, PGENERIC_INPUT_STATE pState)
{
	if (pState == nullptr || dwUserIndex > MAX_CONTROLLERS)
	{
		return ERROR_INVALID_PARAMETER;
	}

	if (ControllerSlots[dwUserIndex].DeviceHandle == nullptr && ControllerSlots[dwUserIndex].conType != XInput)
	{
		if (!ControllerSlots[dwUserIndex].Path.empty()) {
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
		}
		else if (!ControllerSlots[dwUserIndex].BTPath.empty())
		{
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].BTPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
		}
		if (ControllerSlots[dwUserIndex].DeviceHandle == INVALID_HANDLE_VALUE || ControllerSlots[dwUserIndex].DeviceHandle == nullptr)
		{
			return ERROR_DEVICE_NOT_CONNECTED;
		}
	}
	else if (ControllerSlots[dwUserIndex].DeviceHandle == nullptr)
	{
		if (!ControllerSlots[dwUserIndex].XInputPath.empty()) {
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].XInputPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
		}
		else if (!ControllerSlots[dwUserIndex].BTPath.empty())
		{
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].BTPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
		}
		if (ControllerSlots[dwUserIndex].DeviceHandle == INVALID_HANDLE_VALUE || ControllerSlots[dwUserIndex].DeviceHandle == nullptr)
		{
			return ERROR_DEVICE_NOT_CONNECTED;
		}
	}
	switch (ControllerSlots[dwUserIndex].conType)
	{
	case XInput:
	{
		return XboxInput::GetState(&ControllerSlots[dwUserIndex], (XboxInput::GENERIC_INPUT_STATE*)pState);
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
	default: 
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
	}
	return GetLastError();
}
DWORD GenericInput::XInputSetState(DWORD dwUserIndex, PGENERIC_VIBRATION pVibration)
{
	if (pVibration == nullptr || dwUserIndex > 7)
	{
		return ERROR_INVALID_PARAMETER;
	}

	if (ControllerSlots[dwUserIndex].DeviceHandle == nullptr && ControllerSlots[dwUserIndex].conType != XInput)
	{
		if (!ControllerSlots[dwUserIndex].Path.empty()) {
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
		}
		else if (!ControllerSlots[dwUserIndex].BTPath.empty())
		{
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].BTPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
		}
		if (ControllerSlots[dwUserIndex].DeviceHandle == INVALID_HANDLE_VALUE || ControllerSlots[dwUserIndex].DeviceHandle == nullptr)
		{
			return ERROR_DEVICE_NOT_CONNECTED;
		}
	}
	else if (ControllerSlots[dwUserIndex].DeviceHandle == nullptr)
	{
		if (!ControllerSlots[dwUserIndex].XInputPath.empty()) {
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].XInputPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
		}
		else if (!ControllerSlots[dwUserIndex].BTPath.empty())
		{
			ControllerSlots[dwUserIndex].DeviceHandle = CreateFile(ControllerSlots[dwUserIndex].BTPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
		}
		if (ControllerSlots[dwUserIndex].DeviceHandle == INVALID_HANDLE_VALUE || ControllerSlots[dwUserIndex].DeviceHandle == nullptr)
		{
			return ERROR_DEVICE_NOT_CONNECTED;
		}
	}
	switch (ControllerSlots[dwUserIndex].conType)
	{
	case XInput:
	{
		return XboxInput::SetState(&ControllerSlots[dwUserIndex], (XboxInput::GENERIC_VIBRATION*)pVibration);
	}
	case DS://DualSense
	{
		//return DualSense::SetState(&ControllerSlots[dwUserIndex], (DualSense::GENERIC_INPUT_STATE*)pVibration);
	}
	case DS4://DualSense
	{
		//return DualShock4::SetState(&ControllerSlots[dwUserIndex], (DualShock4::GENERIC_INPUT_STATE*)pVibration);
	}
	case NT://Pro controller
	{
		//return ProController::SetState(&ControllerSlots[dwUserIndex], (ProController::GENERIC_INPUT_STATE*)pVibration);
	}
	case SDL:
	{
		break;
	}
	default:
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
	}
	return GetLastError();
}
DWORD GenericInput::GetLayout(DWORD dwUserIndex)
{
	return ControllerSlots[dwUserIndex].conType;
}
DWORD XInputDLL::XInputGetStateEx(DWORD dwUserIndex, PGENERIC_INPUT_STATE pState)
{
	if (funcGetStateEx)
	{
		return funcGetStateEx(dwUserIndex, pState);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
DWORD XInputDLL::XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwFlags, PGENERIC_INPUT_STATE pState)
{
	if (funcWaitForGuideButton)
	{
		return funcWaitForGuideButton(dwUserIndex, dwFlags, pState);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
DWORD XInputDLL::XInputCancelGuideButtonWait(DWORD dwUserIndex)
{
	if (funcCancelGuideButtonWait)
	{
		return funcCancelGuideButtonWait(dwUserIndex);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
DWORD XInputDLL::XInputPowerOffController(DWORD dwUserIndex)
{
	if (funcPowerOffController)
	{
		return funcPowerOffController(dwUserIndex);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
DWORD XInputDLL::XInputGetBaseBusInformation(DWORD dwUserIndex, PGENERIC_CAPABILITIES pCapabilities, PGENERIC_CAPABILITIES pCapabilities2)
{
	if (funcBaseBusInformation)
	{
		return funcBaseBusInformation(dwUserIndex, pCapabilities, pCapabilities2);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
DWORD XInputDLL::XInputGetCapabilitiesEx(DWORD dwUserIndex, PGENERIC_CAPABILITIES pCapabilities, PGENERIC_CAPABILITIES pCapabilities2)
{
	if (pCapabilities == nullptr || pCapabilities2 == nullptr || dwUserIndex > MAX_CONTROLLERS)
	{
		return ERROR_INVALID_PARAMETER;
	}
	if (ControllerSlots[dwUserIndex].XInputPath.empty())
	{
		pCapabilities->Type = 1;
		pCapabilities->SubType = 1;
		pCapabilities->Flags = 0;
		pCapabilities->Gamepad.wButtons = 62463;
		pCapabilities->Gamepad.bLeftTrigger = 255;
		pCapabilities->Gamepad.bRightTrigger = 255;
		pCapabilities->Gamepad.sThumbLX = -64;
		pCapabilities->Gamepad.sThumbLY = -64;
		pCapabilities->Gamepad.sThumbRX = -64;
		pCapabilities->Gamepad.sThumbRY = -64;
		pCapabilities->Vibration.wLeftMotorSpeed = 255;
		pCapabilities->Vibration.wRightMotorSpeed = 255;

		pCapabilities2->Type = 1;
		pCapabilities2->SubType = 1;
		pCapabilities2->Flags = 0;
		pCapabilities2->Gamepad.wButtons = 62463;
		pCapabilities2->Gamepad.bLeftTrigger = 255;
		pCapabilities2->Gamepad.bRightTrigger = 255;
		pCapabilities2->Gamepad.sThumbLX = -64;
		pCapabilities2->Gamepad.sThumbLY = -64;
		pCapabilities2->Gamepad.sThumbRX = -64;
		pCapabilities2->Gamepad.sThumbRY = -64;
		pCapabilities2->Vibration.wLeftMotorSpeed = 255;
		pCapabilities2->Vibration.wRightMotorSpeed = 255;
	}
	else if (funcGetCapabilitiesEx)
	{
		return funcGetCapabilitiesEx(dwUserIndex, pCapabilities, pCapabilities2);
	}
		return ERROR_DEVICE_NOT_CONNECTED;
}

// Documented
DWORD XInputDLL::XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, PGENERIC_CAPABILITIES pCapabilities)
{

	if (pCapabilities == nullptr || dwUserIndex > MAX_CONTROLLERS)
	{
		return ERROR_INVALID_PARAMETER;
	}
	if(ControllerSlots[dwUserIndex].XInputPath.empty())
	{
		pCapabilities->Type = 1;
		pCapabilities->SubType = 1;
		pCapabilities->Flags = 0;
		pCapabilities->Gamepad.wButtons = 62463;
		pCapabilities->Gamepad.bLeftTrigger = 255;
		pCapabilities->Gamepad.bRightTrigger = 255;
		pCapabilities->Gamepad.sThumbLX = -64;
		pCapabilities->Gamepad.sThumbLY = -64;
		pCapabilities->Gamepad.sThumbRX = -64;
		pCapabilities->Gamepad.sThumbRY = -64;
		pCapabilities->Vibration.wLeftMotorSpeed = 255;
		pCapabilities->Vibration.wRightMotorSpeed = 255;

		return ERROR_SUCCESS;
	}

	if (funcGetCapabilities)
	{
		return funcGetCapabilities(dwUserIndex, dwFlags, pCapabilities);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
void XInputDLL::XInputEnable(BOOL enable)
{
	if (funcEnable)
	{
		return funcEnable(enable);
	}
}
DWORD XInputDLL::XInputGetBatteryInformation(DWORD dwUserIndex, BYTE devType, PGENERIC_BATTERY_INFORMATION pBatteryInformation)
{
	if (funcGetBatteryInformation)
	{
		return funcGetBatteryInformation(dwUserIndex, devType, pBatteryInformation);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
DWORD XInputDLL::XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, PGENERIC_KEYSTROKE pKeystroke)
{
	if (funcGetKeystroke)
	{
		return funcGetKeystroke(dwUserIndex, dwReserved, pKeystroke);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
DWORD XInputDLL::XInputGetAudioDeviceIds(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount)
{
	if (funcGetAudioDeviceIds)
	{
		return funcGetAudioDeviceIds(dwUserIndex, pRenderDeviceId, pRenderCount, pCaptureDeviceId, pCaptureCount);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}
DWORD XInputDLL::XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
{
	if (funcGetDSoundGuids)
	{
		return funcGetDSoundGuids(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
	}

	return ERROR_DEVICE_NOT_CONNECTED;
}