// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Window.h"
#include "ControllerScanner.h"
#include <array>

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
        WCHAR path[MAX_PATH] = { '/0' };
        GetSystemDirectoryW(path, MAX_PATH);
        g_xinputModule = LoadLibraryW((std::wstring(path) + L"\\XInput1_4.dll").c_str());
        
        if (g_xinputModule)
        {
            GetProcAddress(g_xinputModule, "XInputGetState");
			GetProcAddress(g_xinputModule, "XInputSetState");
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

