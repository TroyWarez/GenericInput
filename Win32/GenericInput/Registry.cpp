#include "pch.h"
#include "Registry.h"
BOOL Registry::GetBluetoothAddress(GenericInputController& Controller)
{
	HKEY hKeybtEnums = 0;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\BTHENUM", 0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKeybtEnums) != ERROR_SUCCESS || Controller.Path.size() == 0)
	{
		return FALSE;
	}

	WCHAR SubKeyName[1024] = { 0 };
	size_t vidPos = Controller.Path.find_first_of('&');
	size_t pidPos = Controller.Path.find_first_of('&', vidPos + 1);
	if (vidPos == std::string::npos || pidPos == std::string::npos)
	{
		return FALSE;
	}
	std::wstring btHID = Controller.Path.substr(8, Controller.Path.find_first_of('}') - 7);
	std::wstring btHIDSerial = Controller.SerialNum;
	std::transform(btHIDSerial.begin(), btHIDSerial.end(), btHIDSerial.begin(), ::toupper);
	std::wstring newSubKey;
	btHID.append(L"_VID");
	std::wstring vid = Controller.Path.substr(vidPos, 9);
	btHID.append(vid);
	btHID.append(L"_PID");
	std::wstring pid = Controller.Path.substr(pidPos, 5);
	btHID.append(pid);
	DWORD NewSubKeySize = 1024;

	for (DWORD i = 0; ; i++)
	{
	    NewSubKeySize = 1024;
		if (RegEnumKeyExW(hKeybtEnums, i, SubKeyName, &NewSubKeySize, NULL, NULL, NULL, NULL) == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		std::wstring cmpString = SubKeyName;
		if (cmpString == btHID)
		{
			if (RegCloseKey(hKeybtEnums) != ERROR_SUCCESS) {
				return FALSE;
			}

			hKeybtEnums = 0;
			newSubKey = L"SYSTEM\\CurrentControlSet\\Enum\\BTHENUM";
			newSubKey.append(L"\\");
			newSubKey.append(SubKeyName);
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, newSubKey.c_str(), 0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKeybtEnums) != ERROR_SUCCESS)
			{
				return FALSE;
			}
			i = 0;
			if (RegEnumKeyExW(hKeybtEnums, i, SubKeyName, &NewSubKeySize, NULL, NULL, NULL, NULL) == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
			cmpString = SubKeyName;;
		}
		cmpString = cmpString.substr(cmpString.find_last_of('&') + 1, cmpString.find_last_of('_') - cmpString.find_last_of('&') - 1);

		if (cmpString == btHIDSerial)
		{
			if (RegCloseKey(hKeybtEnums) != ERROR_SUCCESS) {
				return FALSE;
			}

			hKeybtEnums = 0;
			newSubKey.append(L"\\");
			newSubKey.append(SubKeyName);
			newSubKey.append(L"\\Device Parameters");
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, newSubKey.c_str(), 0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKeybtEnums) != ERROR_SUCCESS)
			{
				return FALSE;
			}
			ULONGLONG BthAdd = 0;
			DWORD Size = sizeof(ULONGLONG);
			if (RegQueryValueEx(hKeybtEnums, L"BluetoothAddress", NULL, REG_NONE, (LPBYTE)&BthAdd, &Size) == ERROR_SUCCESS)
			{
				Controller.ullbtDeviceInfo = BthAdd;
				return TRUE;
			}
			else
			{
				break;
			}
		}
	}
	return FALSE;
}
