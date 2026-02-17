// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Window.h"
#include "ControllerScanner.h"
#include <array>

// const std::array<std::string, 4> xinputDlls = {
//     "XINPUT1_4.dll",
//     "XINPUT1_3.dll",
//     "XINPUT9_1_0.dll",
//     "XINPUT1_2.dll"
// };

constexpr LPCWSTR XINPUT_DLL = L"\\XInput1_3.dll";
extern Window windowManager;

HMODULE g_xinputModule = nullptr;

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
    case DLL_THREAD_ATTACH:
    {
        if (g_xinputModule == nullptr)
        {
			std::array<WCHAR, MAX_PATH>  path = { L'\0' };
			GetSystemDirectoryW(path.data(), MAX_PATH);
			g_xinputModule = LoadLibraryW((std::wstring(path.data()) + XINPUT_DLL).c_str());
        }
    }
    break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        {
        windowManager.UnregisterWindow();
        if (g_xinputModule)
        {
            FreeLibrary(g_xinputModule);
            g_xinputModule = nullptr;
		}
        break;
        }
    }
    return TRUE;
}

