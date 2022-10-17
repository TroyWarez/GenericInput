#include "pch.h"
#include "GenericInput.h"
#include "GameInput.h"
IGameInput* iGameInput = nullptr;
IGameInputReading* reading = nullptr;
HMODULE hGameInput = nullptr;
typedef HRESULT GameInputCreate(IGameInput** gameInput);
GameInputCreate* GameCreate;

DWORD GameInput::GetState(GenericInputController& controller, GENERIC_INPUT_STATE* pState)
{
	if (hGameInput == nullptr)
	{
#ifdef UNICODE
		std::vector<WCHAR> GameInputPath = { };
		GameInputPath.resize(MAX_PATH);
		if (GetWindowsDirectoryW(GameInputPath.data(), MAX_PATH) != NULL)
		{
			lstrcatW(GameInputPath.data(), L"\\System32\\GameInput.dll");
			hGameInput = LoadLibrary(GameInputPath.data());
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
	else
	{
		// Get the most current gamepad reading, not filtering by a specific device.
		// An error from the GetReading method indicates that no gamepad readings
		// were found.
		if (SUCCEEDED(iGameInput->GetCurrentReading(GameInputKind::GameInputKindAny, nullptr, &reading)))
		{
			// Read the gamepad state.
			GameInputGamepadState state;
			reading->GetGamepadState(&state);

			if (state.buttons & GameInputGamepadA)
			{
				// Logic for the A button being pressed.
			}
		}
	}
	return ERROR_SUCCESS;
}