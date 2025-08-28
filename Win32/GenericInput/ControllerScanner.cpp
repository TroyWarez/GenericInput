#include "pch.h"
#include "ControllerScanner.h"
#include "Devices.h"
#include "controllerDB.h"
#include "Bluetooth.h"
#include "Registry.h"
#include "DualSense.h"
#include "DualShock4.h"

#define WM_CONTROLLER_CONNECTED 0x8007
#define WM_CONTROLLER_DISCONNECTED 0x8008

using namespace Devices;
extern Bluetooth btManager;
void Scanner::ScanForControllers(HWND hWnd, GenericInputController ControllerSlots[])
{
	std::vector<std::wstring> hidDevicePaths;
	std::vector<std::wstring> hidDeviceBusType;
	std::vector<std::wstring> xinputDevicePaths;
	std::vector<std::wstring> xinputDeviceBusType;
	std::vector<std::wstring> btDevicePaths;
	std::vector<HIDD_ATTRIBUTES> xinputHIDArtibs;

	Devices::FindAllDevices(&GUID_DEVINTERFACE_HID, hidDevicePaths, &hidDeviceBusType);
	Devices::FindAllDevices(&GUID_DEVCLASS_XINPUT, xinputDevicePaths, &xinputDeviceBusType);
	for (size_t i = 0; i < xinputDevicePaths.size(); i++)
	{
		HIDD_ATTRIBUTES hidArtib = { 0 };
		std::wstring hidSerial = { 0 };
		if (FindUSBArtibs(xinputDevicePaths[i], hidArtib, hidSerial) == TRUE)
		{
			xinputHIDArtibs.push_back(hidArtib);
		}
	}
	if (hidDeviceBusType.size() != hidDevicePaths.size())
	{
		OutputDebugString(L"CRITICAL ERROR: The number of device paths and bus types aren't the same size!\n");
		return;
	}
	for (size_t i = 0; i < hidDevicePaths.size(); i++)
	{
	    GenericInputController controller = { 0 };

		if (isController(hidDevicePaths[i], controller) == TRUE)
		{
			controller.BusType = hidDeviceBusType[i];
			if (controller.BusType == L"BTHENUM" && controller.SerialNum != L"")
			{
				//Registry::GetBluetoothAddress(controller);
			controller.ullbtDeviceInfo = std::stoll(controller.SerialNum, 0, 16);
			if (btManager.IsBluetoothDeviceConnected(controller.ullbtDeviceInfo) == FALSE)
				{
					controller.ullbtDeviceInfo = 0;
					if (Registry::GetBluetoothAddress(controller) == FALSE)
					{
						OutputDebugString(L"Failed to get the bluetooth address.");
						continue;
					}
					else if (btManager.IsBluetoothDeviceConnected(controller.ullbtDeviceInfo) == FALSE)
					{
						continue;
					}
				}
			}
			if (controller.Atributes.VendorID == 0x054c && controller.Atributes.ProductID == 0x0ce6 ||
				controller.Atributes.VendorID == 0x054c && controller.Atributes.ProductID == 0x0df2 ||
				controller.Atributes.VendorID == 0x054c && controller.Atributes.ProductID == 0x0e5f) {

				if (controller.BusType == L"USB")
				{
					if (DualSense::GetWiredMAC(controller.Path, controller.ullbtDeviceInfo) == FALSE)
					{
						continue;
					}
				}
				controller.conType = DS;
			}
			if (controller.Atributes.VendorID == 0x054c && controller.Atributes.ProductID == 0x05c4 ||
				controller.Atributes.VendorID == 0x054c && controller.Atributes.ProductID == 0x09cc ||
				controller.Atributes.VendorID == 0x054c && controller.Atributes.ProductID == 0x0BA0 ||
				controller.Atributes.VendorID == 0x054c && controller.Atributes.ProductID == 0x0BA1 ) {

				controller.conType = DS4;
			}

			if (controller.Atributes.VendorID == 0x057e && controller.Atributes.ProductID == 0x2009 ||
				controller.Atributes.VendorID == 0x057e && controller.Atributes.ProductID == 0x2069) {
				controller.conType = NT;
			}

			for (size_t j = 0; j < xinputHIDArtibs.size(); j++)
			{
				if (memcmp(&controller.Atributes, &xinputHIDArtibs[j], sizeof(HIDD_ATTRIBUTES)) == 0)
				{
					controller.conType = XInput;
				}
			}
			if (controller.conType == 0)
			{
				controller.conType = SDL;
			}
			for (size_t i = 0; i < MAX_CONTROLLERS; i++)
			{
				if (ControllerSlots[i].Path == controller.Path)
				{
					controller = { 0 };
					break;
				}
				else if (ControllerSlots[i].Path == L"")
				{
					ControllerSlots[i] = controller;
					PostMessageW(hWnd, WM_CONTROLLER_CONNECTED, i, NULL);
					break;
				}
				else if (ControllerSlots[i].ullbtDeviceInfo == controller.ullbtDeviceInfo)
				{
					if (ControllerSlots[i].BusType == L"BTHENUM")
					{
						ControllerSlots[i].BTPath = ControllerSlots[i].Path;
						ControllerSlots[i].Path = controller.Path;
						PostMessageW(hWnd, WM_CONTROLLER_CONNECTED, i, NULL);
						break;
					}
					else if (ControllerSlots[i].BusType == L"USB")
					{
						ControllerSlots[i].Path = controller.Path;
						PostMessageW(hWnd, WM_CONTROLLER_CONNECTED, i, NULL);
						break;
					}
					break;
				}
			}
		}
	}
}
BOOL Scanner::isController(std::wstring& DevicePath, GenericInputController& ControllerData)
{
	HIDD_ATTRIBUTES hidArtib = { 0 };
	std::wstring hidSerial = { 0 };
	if (FindUSBArtibs(DevicePath, hidArtib, hidSerial) == FALSE)
	{
		return FALSE;
	}
	for (size_t i = 0; i < _countof(gamepadDB); i++)
	{
		if (hidArtib.VendorID == gamepadDB[i].VID && hidArtib.ProductID == gamepadDB[i].PID)
		{
			ControllerData.Atributes = hidArtib;
			ControllerData.Path = DevicePath;
			ControllerData.SerialNum = hidSerial;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL Scanner::isControllerConnected(GenericInputController& Controller)
{
	//btManager.isControllerBluetooth(Controller);
	return TRUE;
}
BOOL Scanner::FindUSBArtibs(std::wstring& USBDevicePath, HIDD_ATTRIBUTES& hidArtib, std::wstring& SerialString)
{
	size_t deviceTypePos = USBDevicePath.find_first_of('#');
	if (deviceTypePos == std::string::npos)
	{
		return FALSE;
	}
	std::wstring DevicePathType = USBDevicePath.substr(4, USBDevicePath.find_first_of('#') - 4);
	if (DevicePathType != L"usb" && DevicePathType != L"hid")
	{
		return FALSE;
	}
	if (DevicePathType == L"hid")
	{
		HANDLE hDevice = CreateFile(USBDevicePath.c_str(), (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE), NULL, OPEN_EXISTING, NULL, NULL);
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		else if (HidD_GetAttributes(hDevice, &hidArtib) == TRUE)
		{
			SerialString = L"";
			CloseHandle(hDevice);
			return TRUE;
		}
		else
		{
			CloseHandle(hDevice);
			return FALSE;
		}
	}
	size_t vidPos = USBDevicePath.find_first_of('_', deviceTypePos + 1);
	size_t pidPos = USBDevicePath.find_first_of('_', vidPos + 1);
	if (vidPos == std::string::npos || pidPos == std::string::npos)
	{
		return FALSE;
	}
	std::wstring VID = L"0x";
	VID.append(USBDevicePath.substr(vidPos + 1, 4));
	std::wstring PID = L"0x";
	PID.append(USBDevicePath.substr(pidPos + 1, 4));
	USHORT tVID = std::stoi(VID, 0, 16);
	USHORT tPID = std::stoi(PID, 0, 16);
	if (tVID != 0x00)
	{
		hidArtib.VendorID = tVID;
		hidArtib.ProductID = tPID;
		hidArtib.Size = sizeof(HIDD_ATTRIBUTES);
		hidArtib.VersionNumber = 0x0100;
		return TRUE;
	}
	return FALSE;
}
