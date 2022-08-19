#pragma once
class Scanner
{
public:
	void ScanForControllers(HWND hWnd, GenericInputController ControllerSlots[]);
private:
	BOOL FindUSBArtibs(std::wstring& USBDevicePath, HIDD_ATTRIBUTES& hidArtib, std::wstring& SerialString);
	BOOL isController(std::wstring& DevicePath, GenericInputController& hidArtib);
	BOOL isControllerConnected(GenericInputController& Controller);
	//BOOL GetType(GenericInputController& oiController, std::vector<std::wstring> vxinputDevicePaths);
};

