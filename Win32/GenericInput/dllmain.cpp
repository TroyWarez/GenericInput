// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Window.h"
#include "ControllerScanner.h"
#include <array>
#include <string.h>
#include <Xinput.h>
#include "GenericInput.h"

constexpr size_t NXINPUT_DLL_EXPORTS = 5;
constexpr size_t NXINPUT_DLL_ORDINALS = 7;

constexpr size_t NXINPUT_DLLS = 5;


struct XInputDll
{
    std::array<std::wstring, NXINPUT_DLL_EXPORTS> ExportSymbols;
    std::array<DWORD, NXINPUT_DLL_ORDINALS>  ExportOrdinals;
	std::wstring XInputDllBinPath; // Must be load from the system directory to avoid shenanigans
};

const std::array<XInputDll, NXINPUT_DLLS> XinputDlls = {

    XInputDll{ { L"XInputGetCapabilities", L"XInputEnable", L"XInputGetBatteryInformation", L"XInputGetKeystroke", L"XInputGetAudioDeviceIds" }, 
    { 10, 100, 101, 102, 103, 104, 108 }, L"\\XInput1_4.dll" },

    XInputDll{ { L"XInputGetCapabilities", L"XInputEnable", L"XInputGetDSoundAudioDeviceGuids", L"XInputGetBatteryInformation", L"XInputGetKeystroke" },
    { 100, 101, 102, 103, 0, 0 }, L"\\XInput1_3.dll" },

    XInputDll{ { L"XInputGetCapabilities", L"XInputGetDSoundAudioDeviceGuids", L"", L"", L"" },
    { 0, 0, 0, 0, 0, 0, 0 }, L"\\XInput1_2.dll" },

	XInputDll{ { L"XInputGetCapabilities", L"XInputGetDSoundAudioDeviceGuids", L"", L"", L"" },
	{ 0, 0, 0, 0, 0, 0, 0 }, L"\\XInput1_1.dll" },

    XInputDll{ { L"XInputGetCapabilities", L"XInputEnable", L"XInputGetBatteryInformation", L"XInputGetDSoundAudioDeviceGuids",   L"XInputGetKeystroke" }, 
    { 0, 0, 0, 0, 0, 0, 0 }, L"\\XInput9_1_0.dll" }
};

extern Window windowManager;

HMODULE g_hXinputModule = nullptr;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH: // Hook Window procedure to detect controller changes and scan for controllers when a new window is created
    {
        if (g_hXinputModule == nullptr)
        {
			if (!g_hXinputModule)
			{
				MessageBoxW(nullptr, std::wstring(L"Failed to load" XINPUT_DLL ".Please ensure it is present in the system directory.").c_str(), L"Error", MB_ICONERROR);
				return 1;
			}
        }
    }
    break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        {
        windowManager.UnregisterWindow();
        if (g_hXinputModule)
        {
            FreeLibrary(g_hXinputModule);
            g_hXinputModule = nullptr;
		}
        break;
        }
    }
    return TRUE;
}

