#pragma once
class Bluetooth
{
public:
	BOOL FindBluetoothRadios(std::vector <DEV_BROADCAST_HANDLE>& hBluetoothRadios);
	BOOL IsBluetoothDeviceConnected(BTH_ADDR& btDeviceAddr);
private:
	std::vector <std::wstring> vBluetoothDevicesPaths;
	std::vector <std::wstring> vBluetoothDevicesNames;
};

