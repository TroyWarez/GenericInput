#include "pch.h"
#include "Devices.h"
BOOL Devices::FindAllDevices(const GUID* ClassGuid, std::vector<std::wstring>& DevicePaths, std::vector<std::wstring>* DeviceTypes)
{
	if (DevicePaths.size())
	{
		DevicePaths.clear();
	}
	PSP_DEVICE_INTERFACE_DETAIL_DATA_W deviceDetails = nullptr;
	WCHAR* deviceName = nullptr;
	HDEVINFO hdevInfo = SetupDiGetClassDevs(ClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hdevInfo == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	for (DWORD i = 0; ; i++)
	{
		deviceName = nullptr;
		SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { 0 };
		SP_DEVINFO_DATA deviceData = { 0 };
		ULONG DataSize = 0;
		deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		deviceData.cbSize = sizeof(SP_DEVINFO_DATA);
		if (SetupDiEnumDeviceInterfaces(hdevInfo, NULL, ClassGuid, i, &deviceInterfaceData) == FALSE ||
			SetupDiEnumDeviceInfo(hdevInfo, i, &deviceData) == FALSE)
		{
			if (GetLastError() != ERROR_NO_MORE_ITEMS) {
				SetupDiDestroyDeviceInfoList(hdevInfo);
				return FALSE;
			}
			else
			{
				break;
			}
		}
		if (DeviceTypes != nullptr) {
			DWORD Parent = 0;
			if (CM_Get_Parent(&Parent, deviceData.DevInst, 0) == CR_SUCCESS &&
				CM_Get_DevNode_Registry_Property(Parent, CM_DRP_ENUMERATOR_NAME, NULL, NULL, &DataSize, 0) != CR_NO_SUCH_VALUE)
			{
				if (DataSize) {
					deviceName = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, DataSize * sizeof(WCHAR) + 2);
					if (CM_Get_DevNode_Registry_Property(Parent, CM_DRP_ENUMERATOR_NAME, NULL, deviceName, &DataSize, 0) == CR_SUCCESS)
					{
						if (deviceName != 0) {
							DeviceTypes->push_back(deviceName);
						}
					}
					HeapFree(GetProcessHeap(), 0, deviceName);
				}
			}

		}
		deviceDetails = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)HeapAlloc(GetProcessHeap(), 0, MAX_PATH * sizeof(WCHAR) + sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W));
		if (deviceDetails == NULL)
		{
			SetupDiDestroyDeviceInfoList(hdevInfo);
			return FALSE;
		}
		deviceDetails->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
		if (SetupDiGetDeviceInterfaceDetail(hdevInfo, &deviceInterfaceData, deviceDetails, MAX_PATH * sizeof(WCHAR) + sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W), NULL, NULL) == TRUE)
		{
			std::wstring DevicePath = deviceDetails->DevicePath;
			HeapFree(GetProcessHeap(), 0, deviceDetails);
			DevicePaths.push_back(DevicePath);
		}
	}

	SetupDiDestroyDeviceInfoList(hdevInfo);
	return TRUE;
}