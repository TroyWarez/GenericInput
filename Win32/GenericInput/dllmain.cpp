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
    std::array<std::string, NXINPUT_DLL_EXPORTS> ExportSymbols;
    std::array<DWORD, NXINPUT_DLL_ORDINALS>  ExportOrdinals;
	std::wstring XInputDllBinPath; // Must be load from the system directory to avoid shenanigans
};

const std::array<XInputDll, NXINPUT_DLLS> XinputDlls = {

    XInputDll{ { "XInputGetCapabilities", "XInputEnable", "XInputGetBatteryInformation", "XInputGetKeystroke", "XInputGetAudioDeviceIds" }, 
    { 10, 100, 101, 102, 103, 104, 108 }, L"XINPUT1_4.dll" },

    XInputDll{ { "XInputGetCapabilities", "XInputEnable", "XInputGetDSoundAudioDeviceGuids", "XInputGetBatteryInformation", "XInputGetKeystroke" },
    { 100, 101, 102, 103, 0, 0 }, L"XINPUT1_3.dll" },

    XInputDll{ { "XInputGetCapabilities", "XInputGetDSoundAudioDeviceGuids", "", "", "" },
    { 0, 0, 0, 0, 0, 0, 0 }, L"XINPUT1_2.dll" },

	XInputDll{ { "XInputGetCapabilities", "XInputGetDSoundAudioDeviceGuids", "", "", "" },
	{ 0, 0, 0, 0, 0, 0, 0 }, L"XINPUT1_1.dll" },

    XInputDll{ { "XInputGetCapabilities", "XInputEnable", "XInputGetBatteryInformation", "XInputGetDSoundAudioDeviceGuids",   "XInputGetKeystroke" }, 
    { 0, 0, 0, 0, 0, 0, 0 }, L"XINPUT9_1_0.dll" }
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
			std::array<WCHAR, MAX_PATH>  path = { L'\0' };
			GetSystemDirectoryW(path.data(), MAX_PATH);

			// Parse the export table here to determine which XInput version is present and load the correct one, this is to ensure maximum compatibility with older versions of Windows and avoid loading a newer version of XInput that may not be compatible with the system

            // Assuming 'baseAddress' is the loaded image base (e.g., from GetModuleHandle)
			PBYTE baseAddress = (PBYTE)GetModuleHandle(nullptr);
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)baseAddress;

			// Ensure this is a valid PE file
			if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
				// Calculate the NT Header address
				PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)baseAddress + dosHeader->e_lfanew);

				// Verify the PE signature
				if (ntHeaders->Signature == IMAGE_NT_SIGNATURE) {
					PIMAGE_DATA_DIRECTORY importDirectory = &ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
					// Check if there are actually any imports
					if (importDirectory->Size > 0) {

						// 4. Get the pointer to the first Import Descriptor
						// Note: We must add baseAddress because the header contains Relative Virtual Addresses (RVAs)
						PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(baseAddress + importDirectory->VirtualAddress);

						// 5. Iterate through the descriptors
						// The array is null-terminated (Name will be 0 at the end)
						while (importDescriptor->Name != 0) {

							// --- Get the DLL Name ---
							LPCSTR dllName = (LPCSTR)(baseAddress + importDescriptor->Name);

							// Check if the DLL name matches any of our XInput DLLs
                            for (const auto& dll : XinputDlls) {
                                if (_stricmp(dllName, std::string(dll.XInputDllBinPath.begin(), dll.XInputDllBinPath.end()).c_str()) == 0) {
                                    // Found a match, load this DLL
                                    g_hXinputModule = LoadLibraryW((std::wstring(path.data()) + L"\\" + dll.XInputDllBinPath).c_str());
                                    if (g_hXinputModule) {
                                        for (size_t i = 0; i < dll.ExportSymbols.size(); i++) {
                                            if (!dll.ExportSymbols[i].empty()) {
                                                FARPROC procAddress = GetProcAddress(g_hXinputModule, dll.ExportSymbols[i].c_str());
                                                if (procAddress) {
                                                    // Store the function pointer in the appropriate location (e.g., a global array or struct)
                                                    // For example: g_XInputFunctions[dll.ExportOrdinals[i]] = procAddress;
                                                }
                                                else
                                                {
                                                }
                                            }
                                        }
                                    }
                                }
							}
							importDescriptor++;
						}
					}
				}
			}

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

