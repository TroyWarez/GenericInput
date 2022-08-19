#include "pch.h"
#include "Bluetooth.h"
#include "Devices.h"

#pragma warning( push )
#pragma warning( disable : 4191)

using namespace Devices;
//{00001124-0000-1000-8000-00805F9B34FB}
DEFINE_GUID(UUID_BT_SERVICE_HID, 0x00001124, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
//DEFINE_GUID(GUID_DEVCLASS_XINPUT, 0xec87f1e3, 0xc13b, 0x4100, 0xb5, 0xf7, 0x8b, 0x84, 0xd5, 0x42, 0x60, 0xcb);
BOOL Bluetooth::FindBluetoothRadios(std::vector <DEV_BROADCAST_HANDLE>& hBluetoothRadios)
{
	//FindAllBluetoothHIDDevices();
	if (hBluetoothRadios.size() || FindAllDevices(&GUID_BTHPORT_DEVICE_INTERFACE, vBluetoothDevicesPaths, 0) == FALSE)
	{
		return FALSE;
	}
	for (size_t i = 0; i < vBluetoothDevicesPaths.size(); i++)
	{
		DEV_BROADCAST_HANDLE devBroadcast = { 0 };
		devBroadcast.dbch_size = sizeof(DEV_BROADCAST_HANDLE);
		devBroadcast.dbch_devicetype = DBT_DEVTYP_HANDLE;
		devBroadcast.dbch_handle = CreateFile(vBluetoothDevicesPaths[i].c_str(), (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE), NULL, OPEN_EXISTING, NULL, NULL);
		if (devBroadcast.dbch_handle == INVALID_HANDLE_VALUE)
		{
			hBluetoothRadios[i] = { 0 };
			break;
		}
		hBluetoothRadios.push_back(devBroadcast);
	}
	return TRUE;
}
BOOL Bluetooth::IsBluetoothDeviceConnected(BTH_ADDR& btDeviceAddr)
{
	GUID hidbtGUID = UUID_BT_SERVICE_HID;
	BLUETOOTH_DEVICE_SEARCH_PARAMS btSearchParams = { 0 };
	std::vector<BLUETOOTH_DEVICE_INFO> btDevices;
	BLUETOOTH_DEVICE_INFO btDeviceInfo = { 0 };
	btDeviceInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	btSearchParams.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	btSearchParams.fReturnConnected = TRUE;
	btSearchParams.cTimeoutMultiplier = 1;
	HBLUETOOTH_DEVICE_FIND hDevice = BluetoothFindFirstDevice(&btSearchParams, &btDeviceInfo);
	if (hDevice == NULL)
	{
		return FALSE;
	}
	btDevices.push_back(btDeviceInfo);
	for (DWORD i = 0; ; i++)
	{
		btDeviceInfo = { 0 };
		btDeviceInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
		if (BluetoothFindNextDevice(hDevice, &btDeviceInfo) == FALSE)
		{
			BluetoothFindDeviceClose(hDevice);
			break;
		}
		btDevices.push_back(btDeviceInfo);
	}
	for (size_t i = 0; i < btDevices.size(); i++)
	{
		if (btDevices[i].Address.ullLong == btDeviceAddr)
		{
			return TRUE;
		}
	}
	btDeviceInfo = { 0 };
	return FALSE;
}
#pragma warning( pop )