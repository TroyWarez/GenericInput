#include "pch.h"
#include "GenericInput.h"
#include "GameInput.h"
std::vector<WCHAR> GameInputPath;

DWORD GameInput::GetState(GenericInputController& controller, GENERIC_INPUT_STATE* pState)
{
	if (GameInputPath.size() == NULL)
	{
		try {
			GameInputPath.resize(MAX_PATH);
		}
		catch (std::bad_alloc)
		{
			return ERROR_PATH_NOT_FOUND;
		}
		if (GetSystemDirectoryW(GameInputPath.data(), GameInputPath.size()) == NULL)
		{
			return GetLastError();
		}

	}
	else
	{
		
	}

}
