#include "pch.h"
#include "GenericInput.h"
#include "GameInput.h"
#ifdef NTDDI_WIN10_19H1 
IGameInput* iGameInput = nullptr;
HMODULE hGameInput = nullptr;
typedef HRESULT GameInputCreate(IGameInput** gameInput);
GameInputCreate* GameCreate;
#endif

DWORD GameInput::GetState(GenericInputController& controller, GENERIC_INPUT_STATE* pState)
{
#ifdef NTDDI_WIN10_19H1 
	if (hGameInput == nullptr)
	{
#ifdef UNICODE
		std::vector<WCHAR> GameInputPath = { };
		if (GetWindowsDirectoryW(GameInputPath.data(), MAX_PATH) != NULL)
		{
			lstrcatW(GameInputPath.data(), L"\\System32\\GameInput.dll");
			hGameInput = LoadLibraryW(GameInputPath.data());
			if (hGameInput)
			{
				GameCreate = (GameInputCreate*)GetProcAddress(hGameInput, "GameInputCreate");
				GameCreate(&iGameInput);
			}
		}
#else
		std::vector<CHAR> GameInputPath = { };
		if (GetWindowsDirectoryA(GameInputPath.data(), MAX_PATH) != NULL)
		{
			lstrcatA(GameInputPath.data(), L"\\System32\\GameInput.dll");
			hGameInput = LoadLibraryA(GameInputPath.data());
			if (hGameInput)
			{
				GameCreate = (GameInputCreate*)GetProcAddress(hGameInput, "GameInputCreate");
				GameCreate(&iGameInput);
			}
	}
#endif
		
	}
#endif
	return ERROR_SUCCESS;
}