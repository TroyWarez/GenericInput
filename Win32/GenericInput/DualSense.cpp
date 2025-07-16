#include "pch.h"
#include "DualSense.h"
#include "CRC32.h"

#define PS_OUTPUT_CRC32_SEED 0xA2
const USHORT DualSenseVID = 0x054c;
const USHORT DualSensePID = 0x0ce6;
const SHORT XByteToShort[] = { 
-32768,

-32130,-31875,-31620,-31365,
-31110,-30855,-30600,-30345,-30090,-29835,
-29580,-29325,-29070,-28815,-28560,-28305,
-28050,-27795,-27540,-27285,-27030,-26775,
-26520,-26265,-26010,-25755,-25500,-25245,
-24990,-24735,-24480,-24225,-23970,-23715,
-23460,-23205,-22950,-22695,-22440,-22185,
-21930,-21675,-21420,-21165,-20910,-20655,
-20400,-20145,-19890,-19635,-19380,-19125,
-18870,-18615,-18360,-18105,-17850,-17595,
-17340,-17085,-16830,-16575,-16320,-16065,
-15810,-15555,-15300,-15045,-14790,-14535,
-14280,-14025,-13770,-13515,-13260,-13005,
-12750,-12495,-12240,-11985,-11730,-11475,
-11220,-10965,-10710,-10455,-10200,-9945,
-9690,-9435,-9180,-8925,-8670,-8415,
-8160,-7905,-7650,-7395,-7140,-6885,
-6630,-6375,-6120,-5865,-5610,-5355,
-5100,-4845,-4590,-4335,-4080,-3825,
-3570,-3315,-3060,-2805,-2550,-2295,
-2040,-1785,-1530,-1275,-1020,-765,
-510,-255, 0,255,510,765,1020,1275,1530,
1785,2040,2295,2550,2805,3060,
3315,3570,3825,4080,4335,4590,
4845,5100,5355,5610,5865,6120,
6375,6630,6885,7140,7395,7650,
7905,8160,8415,8670,8925,9180,
9435,9690,9945,10200,10455,10710,
10965,11220,11475,11730,11985,12240,
12495,12750,13005,13260,13515,13770,
14025,14280,14535,14790,15045,15300,
15555,15810,16065,16320,16575,16830,
17085,17340,17595,17850,18105,18360,
18615,18870,19125,19380,19635,19890,
20145,20400,20655,20910,21165,21420,
21675,21930,22185,22440,22695,22950,
23205,23460,23715,23970,24225,24480,
24735,24990,25245,25500,25755,26010,
26265,26520,26775,27030,27285,27540,
27795,28050,28305,28560,28815,29070,
29325,29580,29835,30090,30345,30600,
30855,31110,31365,31620,31875,32130,
32386,

32767
};
const SHORT YByteToShort[] = {
32767,

32130,31875,31620,31365,
31110,30855,30600,30345,30090,29835,
29580,29325,29070,28815,28560,28305,
28050,27795,27540,27285,27030,26775,
26520,26265,26010,25755,25500,25245,
24990,24735,24480,24225,23970,23715,
23460,23205,22950,22695,22440,22185,
21930,21675,21420,21165,20910,20655,
20400,20145,19890,19635,19380,19125,
18870,18615,18360,18105,17850,17595,
17340,17085,16830,16575,16320,16065,
15810,15555,15300,15045,14790,14535,
14280,14025,13770,13515,13260,13005,
12750,12495,12240,11985,11730,11475,
11220,10965,10710,10455,10200,9945,
9690,9435,9180,8925,8670,8415,
8160,7905,7650,7395,7140,6885,
6630,6375,6120,5865,5610,5355,
5100,4845,4590,4335,4080,3825,
3570,3315,3060,2805,2550,2295,
2040,1785,1530,1275,1020,765,
510,255, 0,-255,-510,-765,-1020,-1275,-1530,
-1785,-2040,-2295,-2550,-2805,-3060,
-3315,-3570,-3825,-4080,-4335,-4590,
-4845,-5100,-5355,-5610,-5865,-6120,
-6375,-6630,-6885,-7140,-7395,-7650,
-7905,-8160,-8415,-8670,-8925,-9180,
-9435,-9690,-9945,-10200,-10455,-10710,
-10965,-11220,-11475,-11730,-11985,-12240,
-12495,-12750,-13005,-13260,-13515,-13770,
-14025,-14280,-14535,-14790,-15045,-15300,
-15555,-15810,-16065,-16320,-16575,-16830,
-17085,-17340,-17595,-17850,-18105,-18360,
-18615,-18870,-19125,-19380,-19635,-19890,
-20145,-20400,-20655,-20910,-21165,-21420,
-21675,-21930,-22185,-22440,-22695,-22950,
-23205,-23460,-23715,-23970,-24225,-24480,
-24735,-24990,-25245,-25500,-25755,-26010,
-26265,-26520,-26775,-27030,-27285,-27540,
-27795,-28050,-28305,-28560,-28815,-29070,
-29325,-29580,-29835,-30090,-30345,-30600,
-30855,-31110,-31365,-31620,-31875,-32130,
-32386,
-32768
};
BOOL DualSense::GetWiredMAC(std::wstring& DevicePath, BTH_ADDR& ullbtDeviceInfo)
{
	HANDLE DeviceH = CreateFile(DevicePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, NULL, NULL);
	if (DeviceH == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	HIDD_ATTRIBUTES DeviceAttrib = { 0 };
	if (HidD_GetAttributes(DeviceH, &DeviceAttrib) == FALSE || (DeviceAttrib.VendorID != DualSenseVID  && DeviceAttrib.ProductID != DualSensePID))
	{
		return FALSE;
	}
	PHIDP_PREPARSED_DATA PreparsedData = nullptr;
	if (HidD_GetPreparsedData(DeviceH, &PreparsedData) == FALSE)
	{
		return FALSE;
	}
	HIDP_CAPS deviceCaps = { 0 };

	if (HidP_GetCaps(PreparsedData, &deviceCaps) == HIDP_STATUS_SUCCESS && deviceCaps.InputReportByteLength != 64) {
		return FALSE;
	}
	if (PreparsedData)
	{
		HidD_FreePreparsedData(PreparsedData);
	}
	BYTE ControllerMAC[20] = { 0 };
	ControllerMAC[0] = 0x09;
	if (HidD_GetFeature(DeviceH, ControllerMAC, 20) == FALSE)
	{
		return FALSE;
	}
	if (DeviceH)
	{
		CloseHandle(DeviceH);
	}
	memcpy(&ullbtDeviceInfo, &ControllerMAC[1], 6);
	return TRUE;
}
DWORD DualSense::GetState(GenericInputController& controller, GENERIC_INPUT_STATE* pState)
{
	if (controller.DeviceHandle == 0 || controller.DeviceHandle == INVALID_HANDLE_VALUE || pState == nullptr)
	{
		return ERROR_INVALID_PARAMETER;
	}
	DWORD Flags = 0;
	if (GetHandleInformation(controller.DeviceHandle, &Flags) == FALSE)
	{
		return GetLastError();
	}
	if (controller.dwPacketNumber < 10241)
	{
		controller.dwPacketNumber++;
	}
	else
	{
		controller.dwPacketNumber = 0L;
	}
	pState->dwPacketNumber = controller.dwPacketNumber;
	if (controller.InputBufferSize == 0)
	{
		PHIDP_PREPARSED_DATA pData = nullptr;
		HIDP_CAPS deviceCaps = { 0 };
		controller.InputBuffer.clear();
		if (HidD_GetPreparsedData(controller.DeviceHandle, &pData) == FALSE)
		{
			return GetLastError();
		}
		if (HidP_GetCaps(pData, &deviceCaps) != HIDP_STATUS_SUCCESS)
		{
			return GetLastError();
		}
		if (pData)
		{
			HidD_FreePreparsedData(pData);
		}

		controller.InputBufferSize = deviceCaps.InputReportByteLength;
	}

	switch ((connectionType)controller.InputBufferSize)
	{
	case Bluetooth:
	{
		if (controller.InputBuffer.size() != controller.InputBufferSize)
		{
			try {
				controller.InputBuffer.resize(controller.InputBufferSize);
				controller.InputBuffer[0] = 0x31;
			}
			catch (std::bad_alloc)
			{
				return ERROR_GEN_FAILURE;
			}
		}
// 		if (ReadFile(controller.DeviceHandle, controller.InputBuffer.data(), (DWORD)controller.InputBuffer.size(), NULL, NULL) == FALSE)
// 		{
// 			return GetLastError();
// 		}
		if (HidD_GetInputReport(controller.DeviceHandle, controller.InputBuffer.data(), (DWORD)controller.InputBufferSize))
		{


		if (controller.InputBuffer[0] == 0x31)
		{
			pState->Gamepad.sThumbLX = XByteToShort[controller.InputBuffer[1]];
			pState->Gamepad.sThumbLY = YByteToShort[controller.InputBuffer[2]];
			pState->Gamepad.sThumbRX = XByteToShort[controller.InputBuffer[3]];
			pState->Gamepad.sThumbRY = YByteToShort[controller.InputBuffer[4]];

			pState->Gamepad.bLeftTrigger = controller.InputBuffer[6];
			pState->Gamepad.bRightTrigger = controller.InputBuffer[7];

			switch (controller.InputBuffer[9] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_X;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[9] & 0x0f)
			{
			case 0x0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT | CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT | CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x4:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT | CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT | CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			}

			switch (controller.InputBuffer[10] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK | CONTROLLER_BUTTON_START;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[10] & 0x0f)
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x9:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xA:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0xB:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xD:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xE:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0xF:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[11] & 0x0f)//Mic button and touch pad button need to be dealt with differently
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x2:
			{
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x4:
			{
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x6:
			{
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			}

			break;
		}
				}
		break;
	}
	case USB:
	{
		if (controller.InputBuffer.size() != controller.InputBufferSize)
		{
			try {
				controller.InputBuffer.resize(controller.InputBufferSize);
			}
			catch (std::bad_alloc)
			{
				return ERROR_GEN_FAILURE;
			}
		}
		if (ReadFile(controller.DeviceHandle, controller.InputBuffer.data(), (DWORD)controller.InputBuffer.size(), NULL, NULL) == FALSE)
		{
			return GetLastError();
		}

		if (controller.InputBuffer[0] == 0x01)
		{
			pState->Gamepad.sThumbLX = XByteToShort[controller.InputBuffer[1]];
			pState->Gamepad.sThumbLY = YByteToShort[controller.InputBuffer[2]];
			pState->Gamepad.sThumbRX = XByteToShort[controller.InputBuffer[3]];
			pState->Gamepad.sThumbRY = YByteToShort[controller.InputBuffer[4]];

			pState->Gamepad.bLeftTrigger = controller.InputBuffer[5];
			pState->Gamepad.bRightTrigger = controller.InputBuffer[6];

			switch (controller.InputBuffer[8] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_X;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_A | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_X;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_Y | CONTROLLER_BUTTON_X | CONTROLLER_BUTTON_B | CONTROLLER_BUTTON_A;
				break;
			}
				break;
			}

			switch (controller.InputBuffer[8] & 0x0f)
			{
			case 0x0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT | CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_RIGHT | CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x4:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT | CONTROLLER_BUTTON_DPAD_DOWN;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_DPAD_LEFT | CONTROLLER_BUTTON_DPAD_UP;
				break;
			}
			}

			switch (controller.InputBuffer[9] & 0xf0)
			{
			case 0x10:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x20:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START;
				break;
			}
			case 0x30:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x40:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0x50:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x60:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0x70:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0x80:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK;
				break;
			}
			case 0x90:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xA0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0xB0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_START | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xC0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK;
				break;
			}
			case 0xD0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK;
				break;
			}
			case 0xE0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_START;
				break;
			}
			case 0xF0:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSTICK | CONTROLLER_BUTTON_LEFTSTICK | CONTROLLER_BUTTON_BACK | CONTROLLER_BUTTON_START;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[9] & 0x0f)
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x2:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x6:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0x9:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xA:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0xB:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xD:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			case 0xE:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER;
				break;
			}
			case 0xF:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_RIGHTSHOULDER | CONTROLLER_BUTTON_LEFTSHOULDER;
				break;
			}
			break;
			}

			switch (controller.InputBuffer[10] & 0x0f)//Mic button and touch pad button needed to be dealt with differently
			{
			case 0x1:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x2:
			{
				break;
			}
			case 0x3:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x4:
			{
				break;
			}
			case 0x5:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			case 0x6:
			{
				break;
			}
			case 0x7:
			{
				pState->Gamepad.wButtons |= CONTROLLER_BUTTON_GUIDE;
				break;
			}
			}

			break;
		}

		break;
	}
	default:
	{
		return ERROR_GEN_FAILURE;
	}
	}
	return ERROR_SUCCESS;
}
DWORD DualSense::SetState(GenericInputController& controller, LPVOID* pData, DWORD dSize)
{
	if (dSize > 512)
	{
		return ERROR_INVALID_PARAMETER;
	}
	try {
		OutputBuffer.resize(dSize);
	}
	catch (std::bad_alloc)
	{
		return ERROR_OUTOFMEMORY;
	}

	CopyMemory(OutputBuffer.data(), pData, dSize);
	uint32_t CRC32 = GetCRC32(OutputBuffer, CRC32_SEED);

	return ERROR_SUCCESS;
}
