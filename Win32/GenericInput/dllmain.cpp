// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Window.h"
#include "ControllerScanner.h"
#include <array>
#include <string.h>
#include <Xinput.h>
#include "GenericInput.h"

constexpr int NXINPUT_DLL_EXPORTS = 5;
constexpr int NXINPUT_DLL_ORDINALS = 7;

constexpr int NXINPUT_DLLS = 5;

constexpr int ORDINAL_100_GET_STATE_EX = 100;
constexpr int ORDINAL_101_WAIT_FOR_GUIDE_BUTTON = 101;
constexpr int ORDINAL_102_CANCEL_GUIDE_BUTTON_WAIT = 102;
constexpr int ORDINAL_103_POWER_OFF_CONTROLLER = 103;
constexpr int ORDINAL_104_GET_BASE_BUS_INFORMATION = 104;
constexpr int ORDINAL_108_GET_CAPABILITIES_EX = 108;

HMODULE g_hXinputModule = nullptr;

pXInputGetStateEx funcGetStateEx = nullptr;
pXInputWaitForGuideButton  funcWaitForGuideButton = nullptr;
pXInputCancelGuideButtonWait  funcCancelGuideButtonWait = nullptr;
pXInputPowerOffController  funcPowerOffController = nullptr;
pXInputGetBaseBusInformation  funcBaseBusInformation = nullptr;
pXInputGetCapabilitiesEx funcGetCapabilitiesEx = nullptr;

pXInputGetCapabilities funcGetCapabilities = nullptr;
pXInputEnable funcEnable = nullptr;
pXInputGetBatteryInformation funcGetBatteryInformation = nullptr;
pXInputGetKeystroke funcGetKeystroke = nullptr;
pXInputGetAudioDeviceIds funcGetAudioDeviceIds = nullptr;
pXInputGetDSoundAudioDeviceGuids funcGetDSoundGuids = nullptr;

struct XInputDll
{
	std::array<std::string, NXINPUT_DLL_EXPORTS> ImportSymbols;
	std::array<DWORD, NXINPUT_DLL_ORDINALS> ImportOrdinals;
	std::wstring XInputDllBinPath; // Must be load from the system directory to avoid shenanigans
};

const std::array<XInputDll, NXINPUT_DLLS> XinputDlls = {

    XInputDll{ { "XInputGetCapabilities", "XInputEnable", "XInputGetBatteryInformation", "XInputGetKeystroke", "XInputGetAudioDeviceIds" }, 
    { ORDINAL_100_GET_STATE_EX,
	  ORDINAL_101_WAIT_FOR_GUIDE_BUTTON,
	  ORDINAL_102_CANCEL_GUIDE_BUTTON_WAIT,
	  ORDINAL_103_POWER_OFF_CONTROLLER,
	  ORDINAL_104_GET_BASE_BUS_INFORMATION,
	  ORDINAL_108_GET_CAPABILITIES_EX,
	  NULL }, L"XINPUT1_4.dll" },

    XInputDll{ { "XInputGetCapabilities", "XInputEnable", "XInputGetDSoundAudioDeviceGuids", "XInputGetBatteryInformation", "XInputGetKeystroke" },
    { ORDINAL_100_GET_STATE_EX,
	  ORDINAL_101_WAIT_FOR_GUIDE_BUTTON,
	  ORDINAL_102_CANCEL_GUIDE_BUTTON_WAIT,
	  ORDINAL_103_POWER_OFF_CONTROLLER,
	  NULL, NULL }, L"XINPUT1_3.dll" },

    XInputDll{ { "XInputGetCapabilities", "XInputGetDSoundAudioDeviceGuids", "", "", "" },
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL }, L"XINPUT1_2.dll" },

	XInputDll{ { "XInputGetCapabilities", "XInputGetDSoundAudioDeviceGuids", "", "", "" },
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL }, L"XINPUT1_1.dll" },

    XInputDll{ { "XInputGetCapabilities", "XInputEnable", "XInputGetBatteryInformation", "XInputGetDSoundAudioDeviceGuids",   "XInputGetKeystroke" }, 
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL }, L"XINPUT9_1_0.dll" }
};

extern Window windowManager;


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
                                        for (size_t i = 0; i < dll.ImportSymbols.size(); i++) {
                                            if (!dll.ImportSymbols[i].empty()) {
                                                for (size_t j = 0; j < dll.ImportSymbols.size(); j++) {
                                                    if (_stricmp("XInputGetCapabilities", dll.ImportSymbols[j].c_str()) == 0) {
														funcGetCapabilities =
															(pXInputGetCapabilities)GetProcAddress(g_hXinputModule, "XInputGetCapabilities");
                                                    }

													if (_stricmp("XInputEnable", dll.ImportSymbols[j].c_str()) == 0) {
														funcEnable =
															(pXInputEnable)GetProcAddress(g_hXinputModule, "XInputEnable");
													}

													if (_stricmp("XInputGetBatteryInformation", dll.ImportSymbols[j].c_str()) == 0) {
														funcGetBatteryInformation =
															(pXInputGetBatteryInformation)GetProcAddress(g_hXinputModule, "XInputGetBatteryInformation");
													}

													if (_stricmp("XInputGetKeystroke", dll.ImportSymbols[j].c_str()) == 0) {
														funcGetKeystroke =
															(pXInputGetKeystroke)GetProcAddress(g_hXinputModule, "XInputGetKeystroke");
													}

													if (_stricmp("XInputGetAudioDeviceIds", dll.ImportSymbols[j].c_str()) == 0) {
														funcGetAudioDeviceIds =
															(pXInputGetAudioDeviceIds)GetProcAddress(g_hXinputModule, "XInputGetAudioDeviceIds");
													}

													if (_stricmp("XInputGetDSoundAudioDeviceGuids", dll.ImportSymbols[j].c_str()) == 0) {
														funcGetDSoundGuids = (pXInputGetDSoundAudioDeviceGuids)GetProcAddress(g_hXinputModule, "XInputGetDSoundAudioDeviceGuids");
													}
                                                }
                                                FARPROC procAddress = GetProcAddress(g_hXinputModule, dll.ImportSymbols[i].c_str());
                                                if (procAddress) {
                                                    // Store the function pointer in the appropriate location (e.g., a global array or struct)
                                                    // For example: g_XInputFunctions[dll.ExportOrdinals[i]] = procAddress;
                                                }
                                                else
                                                {
                                                }
                                            }
                                        }

										for (size_t i = 0; i < dll.ImportOrdinals.size(); i++) {
											if (dll.ImportOrdinals[i] != NULL) {
												switch (dll.ImportOrdinals[i]) {
													case ORDINAL_100_GET_STATE_EX:
														funcGetStateEx = (pXInputGetStateEx)GetProcAddress(g_hXinputModule, MAKEINTRESOURCEA(ORDINAL_100_GET_STATE_EX));
														break;
													case ORDINAL_101_WAIT_FOR_GUIDE_BUTTON:
														funcWaitForGuideButton = (pXInputWaitForGuideButton)GetProcAddress(g_hXinputModule, MAKEINTRESOURCEA(ORDINAL_101_WAIT_FOR_GUIDE_BUTTON));
														break;
													case ORDINAL_102_CANCEL_GUIDE_BUTTON_WAIT:
														funcCancelGuideButtonWait = (pXInputCancelGuideButtonWait)GetProcAddress(g_hXinputModule, MAKEINTRESOURCEA(ORDINAL_102_CANCEL_GUIDE_BUTTON_WAIT));
														break;
													case ORDINAL_103_POWER_OFF_CONTROLLER:
														funcPowerOffController = (pXInputPowerOffController)GetProcAddress(g_hXinputModule, MAKEINTRESOURCEA(ORDINAL_103_POWER_OFF_CONTROLLER));
														break;
													case ORDINAL_104_GET_BASE_BUS_INFORMATION:
														funcBaseBusInformation = (pXInputGetBaseBusInformation)GetProcAddress(g_hXinputModule, MAKEINTRESOURCEA(ORDINAL_104_GET_BASE_BUS_INFORMATION));
														break;
													case ORDINAL_108_GET_CAPABILITIES_EX:
														funcGetCapabilitiesEx = (pXInputGetCapabilitiesEx)GetProcAddress(g_hXinputModule, MAKEINTRESOURCEA(ORDINAL_108_GET_CAPABILITIES_EX));
														break;
													default:
														break;
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

