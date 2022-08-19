#pragma once
namespace Devices
{
	BOOL FindAllDevices(const GUID* ClassGuid, std::vector<std::wstring>& DevicePaths, std::vector<std::wstring>* DeviceBusTypes);
};

