#include "framework.h"
#include "File.h"
#define VID_POS_1 8
#define VID_POS_2 10
#define PID_POS 18

typedef struct _GAMEPAD_DATA {
	std::string  bLeftTrigger;
	std::string  bRightTrigger;
	std::string TriggerType;
	std::string sThumbLX;
	std::string sThumbLY;
	std::string sThumbRX;
	std::string sThumbRY;
	std::string bButtonA;
	std::string bButtonB;
	std::string bButtonX;
	std::string bButtonY;
	std::string bButtonBack;
	std::string bButtonGuide;
	std::string bButtonStart;
	std::string bButtonLeftStick;
	std::string bButtonRightStick;
	std::string bButtonLeftShoulder;
	std::string bButtonRightShoulder;
	std::string bButtonDpadUp;
	std::string bButtonDpadDown;
	std::string bButtonDpadLeft;
	std::string bButtonDpadRight;

} GAMEPAD_DATA, * P_GAMEPAD_DATA;
#pragma warning( push )
#pragma warning( disable : 5045)
#pragma warning( disable : 4710)
#pragma warning( disable : 4711)
#ifdef UNICODE
bool File::ShouldHeaderFileBeRebuiltW(std::wstring Path)
{
#ifdef WINDOWS

	SYSTEMTIME sysTime = { 0 };
	SYSTEMTIME fileSysTime = { 0 };
	FILETIME fileTime = { 0 };
	LARGE_INTEGER FileSize = { };

	HANDLE hHeader = nullptr;
	hHeader = CreateFileW(Path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, NULL, NULL);
	GetSystemTime(&sysTime);

	if (hHeader == INVALID_HANDLE_VALUE)
	{
		return true;
	}

	if (GetFileSizeEx(hHeader, &FileSize) == false)
	{
		return true;
	}
	if ((FileSize.QuadPart == 0) || (FileSize.QuadPart < SDL_DATABASE_MAX_SIZE))
	{

		return true;
	}

	if (GetFileTime(hHeader, 0, 0, &fileTime) == false)
	{
		return true;
	}

	if (CloseHandle(hHeader) == false)
	{
		return true;
	}
	if (FileTimeToSystemTime(&fileTime, &fileSysTime) == false)
	{
		return true;
	}

	if (fileSysTime.wDay != sysTime.wDay)
	{
		return true;
	}
#endif
	return true;
}
bool File::CreateHeaderFromStringW(std::wstring Path, std::string& Raw_SDL_DB)
{
	std::string HeaderData;
#ifdef WINDOWS
	HeaderData = " \t// ";
	SYSTEMTIME sysTime = { };
	FILETIME fileSysTime = { };
	GetLocalTime(&sysTime);
	HeaderData.append(std::to_string(sysTime.wYear));
	HeaderData.append("-");
	HeaderData.append(std::to_string(sysTime.wMonth));
	HeaderData.append("-");
	HeaderData.append(std::to_string(sysTime.wDay));
	HeaderData.append(" ");

	HeaderData.append(std::to_string(sysTime.wHour));
	HeaderData.append(":");
	HeaderData.append(std::to_string(sysTime.wMinute));

	// You can change the language translation here.
	switch (sysTime.wDayOfWeek)
	{
		case 0:
		{
			HeaderData.append(" Sunday");
			break;
		}
		case 1:
		{
			HeaderData.append(" Monday");
			break;
		}
		case 2:
		{
			HeaderData.append(" Tuesday");
			break;
		}
		case 3:
		{
			HeaderData.append(" Wednesday");
			break;
		}
		case 4:
		{
			HeaderData.append(" Thursday");
			break;
		}
		case 5:
		{
			HeaderData.append(" Friday");
			break;
		}
		case 6:
		{
			HeaderData.append(" Saturday");
			break;
		}
		default:
		{
#ifdef UNICODE
			std::wstring DebugString = L"Unknown day of the week: ";
			DebugString.append(std::to_wstring(sysTime.wDayOfWeek));
#else
			std::string DebugString = "Unknown day of the week: ";
			DebugString.append(std::to_string(sysTime.wDayOfWeek));
#endif // UNICODE
			OutputDebugString(DebugString.c_str());
			break;
		}
	}
#endif // WIN32
	HeaderData.append(" \n#pragma once\n#include \"pch.h\"\nconst GAMEPAD_DATA gamepadDB[] = {\n");
	size_t StartPos = 0;
	size_t Pos = 0;
	while (Pos < Raw_SDL_DB.size()) {
		Pos = Raw_SDL_DB.find_first_of('\n', Pos);
		if (Pos == std::string::npos)
		{
			return FALSE;
		}
		else if (Pos == StartPos)
		{
			Pos++;
		}
		else {
			std::string ControllerData = Raw_SDL_DB.substr(StartPos, (Pos - StartPos));
			size_t ConPos = 0;
			size_t StartConPos = 0;
			std::string gamepadName;
			std::string Vid;
			std::string Pid;
			GAMEPAD_DATA gamepadData = { };
			while (ConPos < ControllerData.size()) {
				ConPos = ControllerData.find_first_of(',', ConPos);
				if (ConPos == std::string::npos)
				{
					break;
				}
				else if (ConPos == StartConPos)
				{
					ConPos++;
				}
				else
				{
					std::string SubData = ControllerData.substr(StartConPos, (ConPos - StartConPos));
					size_t ConPosButton = SubData.find_first_of(':');
					if (ConPosButton == std::string::npos && SubData.size() == 32 && StartConPos == 0)//Should be sdl vid & pid encoded string
					{
						if (SubData[1] == '5')
						{
							Pid = SubData.substr(PID_POS, 4);
						}
						else if (SubData[1] == '3')
						{
							Pid =  SubData.substr(18, 2) + SubData.substr(16, 2);
						}
						else
						{
							Vid = "";
							Pid = "";
						}
						Vid =  SubData.substr(VID_POS_2, 2) + SubData.substr(VID_POS_1, 2);
					}
					else if (ConPosButton == std::string::npos && ConPos != ControllerData.size())//Should be the gamepad name
					{
						if (Vid == Pid)
						{
							gamepadName = "";
						}
						else
						{
							gamepadName = SubData;
						}
					}
					else
					{
						std::string DataType = SubData.substr(0, ConPosButton);
						std::string Mapping = SubData.substr(ConPosButton + 1);
						if (Mapping.find_first_of('~') != std::string::npos)
						{
							Mapping = Mapping.substr(1, Mapping.find_first_of('~') - 1);
						}
						else if (Mapping[1] == '0' && Mapping.length() == 3)
						{
							Mapping = "b" + Mapping.substr(2, Mapping.find_first_of('0') + 1);
						}


						if (DataType == "a")
						{
							gamepadData.bButtonA = Mapping;
						}
						else if (DataType == "b")
						{
							gamepadData.bButtonB = Mapping;

						}
						else if (DataType == "x")
						{
							gamepadData.bButtonX = Mapping;
						}
						else if (DataType == "y")
						{
							gamepadData.bButtonY = Mapping;
						}
						else if (DataType == "dpdown")
						{
							gamepadData.bButtonDpadDown = Mapping;
						}
						else if (DataType == "dpleft")
						{
							gamepadData.bButtonDpadLeft = Mapping;
						}
						else if (DataType == "dpright")
						{
							gamepadData.bButtonDpadRight = Mapping;
						}
						else if (DataType == "dpup")
						{
							gamepadData.bButtonDpadUp = Mapping;
						}
						else if (DataType == "guide")
						{
							gamepadData.bButtonGuide = Mapping;
						}
						else if (DataType == "leftshoulder")
						{
							gamepadData.bButtonLeftShoulder = Mapping;
						}
						else if (DataType == "leftstick")
						{
							gamepadData.bButtonLeftStick = Mapping;
						}
						else if (DataType == "lefttrigger")
						{
							gamepadData.bLeftTrigger = Mapping;
						}
						else if (DataType == "leftx" || DataType == "+leftx" || DataType == "-leftx")
						{
							gamepadData.sThumbLX = Mapping;
						}
						else if (DataType == "lefty" || DataType == "+lefty" || DataType == "-lefty")
						{
							gamepadData.sThumbLY = Mapping;
						}
						else if (DataType == "rightshoulder")
						{
							gamepadData.bButtonRightShoulder = Mapping;
						}
						else if (DataType == "rightstick")
						{
							gamepadData.bButtonRightStick = Mapping;
						}
						else if (DataType == "righttrigger")
						{
							gamepadData.bRightTrigger = Mapping;
						}
						else if (DataType == "rightx" || DataType == "+rightx" || DataType == "-rightx")
						{
							gamepadData.sThumbRX = Mapping;
						}
						else if (DataType == "righty" || DataType == "+righty" || DataType == "-righty")
						{
							gamepadData.sThumbRY = Mapping;
						}
						else if (DataType == "start")
						{
							gamepadData.bButtonStart = Mapping;
						}
						else if (DataType == "back")
						{
							gamepadData.bButtonBack = Mapping;
						}
						else if (DataType == "platform")
						{
							if (Mapping == "Android" || Mapping == "iOS")
							{
								goto Done;
							}
							break;
						}
#ifdef _DEBUG
						else
						{
#ifdef WINDOWS
							OutputDebugString(L"Unknown data type: \n\"");
							OutputDebugStringA(DataType.c_str());
							OutputDebugString(L"\"\n");
#endif // WIN32
						}
#endif
					}

				}
				StartConPos = ConPos;
			}
			if (gamepadName.size() && Vid != "" && Pid != "")
			{
				HeaderData.append(" { 0x");
				HeaderData.append(Vid);
				HeaderData.append(", 0x");
				HeaderData.append(Pid);
				HeaderData.append(", L\"");

				HeaderData.append(gamepadName);
				HeaderData.append("\", ");
				if (gamepadData.bLeftTrigger.find_first_of('a') == std::string::npos)
				{
					if (gamepadData.bLeftTrigger.find_first_of('b') != std::string::npos)
					{
						HeaderData.append(gamepadData.bLeftTrigger.substr(gamepadData.bLeftTrigger.find_first_of('b') + 1));
						HeaderData.append(", ");
					}
					else
					{
						HeaderData.append("0xFF, ");
					}
				}
				else
				{
					HeaderData.append(gamepadData.bLeftTrigger.substr(gamepadData.bLeftTrigger.find_first_of('a') + 1));
					HeaderData.append(", ");
				}

				if (gamepadData.bRightTrigger.find_first_of('a') == std::string::npos)
				{
					if (gamepadData.bRightTrigger.find_first_of('b') != std::string::npos)
					{
						HeaderData.append(gamepadData.bRightTrigger.substr(gamepadData.bRightTrigger.find_first_of('b') + 1));
						HeaderData.append(", ");
					}
					else
					{
						HeaderData.append("0xFF, ");
					}
				}
				else
				{
					HeaderData.append(gamepadData.bRightTrigger.substr(gamepadData.bRightTrigger.find_first_of('a') + 1));
					HeaderData.append(", ");
				}

				if (gamepadData.bRightTrigger.find_first_of('a') == std::string::npos || gamepadData.bLeftTrigger.find_first_of('a') == std::string::npos)
				{
					if (gamepadData.bRightTrigger.find_first_of('b') != std::string::npos || gamepadData.bLeftTrigger.find_first_of('b') != std::string::npos)
					{
						HeaderData.append("L\"b\", ");
					}
					else
					{
						HeaderData.append("L\"\", ");
					}
				}
				else
				{
					HeaderData.append("L\"a\", ");
				}

				if (gamepadData.sThumbLX.find_first_of('a') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.sThumbLX.substr(gamepadData.sThumbLX.find_first_of('a') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.sThumbLY.find_first_of('a') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.sThumbLY.substr(gamepadData.sThumbLY.find_first_of('a') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.sThumbRX.find_first_of('a') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.sThumbRX.substr(gamepadData.sThumbRX.find_first_of('a') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.sThumbRY.find_first_of('a') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.sThumbRY.substr(gamepadData.sThumbRY.find_first_of('a') + 1));
					HeaderData.append(", ");
				}

				if (gamepadData.bButtonA.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonA.substr(gamepadData.bButtonA.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonB.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonB.substr(gamepadData.bButtonB.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonX.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonX.substr(gamepadData.bButtonX.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonY.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonY.substr(gamepadData.bButtonY.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonBack.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonBack.substr(gamepadData.bButtonBack.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonGuide.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonGuide.substr(gamepadData.bButtonGuide.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonStart.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonStart.substr(gamepadData.bButtonStart.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonLeftStick.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonLeftStick.substr(gamepadData.bButtonLeftStick.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonRightStick.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonRightStick.substr(gamepadData.bButtonRightStick.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonLeftShoulder.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonLeftShoulder.substr(gamepadData.bButtonLeftShoulder.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonRightShoulder.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonRightShoulder.substr(gamepadData.bButtonRightShoulder.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonDpadUp.find_first_of('.') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonDpadUp.substr(gamepadData.bButtonDpadUp.find_first_of('.') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonDpadDown.find_first_of('.') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonDpadDown.substr(gamepadData.bButtonDpadDown.find_first_of('.') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonDpadLeft.find_first_of('.') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonDpadLeft.substr(gamepadData.bButtonDpadLeft.find_first_of('.') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonDpadRight.find_first_of('.') == std::string::npos)
				{
					HeaderData.append("0xFF },");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonDpadRight.substr(gamepadData.bButtonDpadRight.find_first_of('.') + 1));
					HeaderData.append(" },");
				}
				HeaderData.append("\n");
			}
		}
		StartPos = Pos;
	}
Done:
	HeaderData.append(" };");
	HANDLE hHeader = CreateFileW(Path.c_str(), GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, NULL, NULL);
	if (hHeader == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(L"Failed to create header file!");
		return FALSE;
	}
	if (WriteFile(hHeader, HeaderData.data(), (DWORD)HeaderData.length(), NULL, NULL) == FALSE)
	{
		CloseHandle(hHeader);
		OutputDebugString(L"Failed to write header file!");
		return FALSE;
	}
	CloseHandle(hHeader);
	return TRUE;
}
#else
bool File::ShouldHeaderFileBeRebuiltA(std::string Path)
{
#ifdef WINDOWS

	SYSTEMTIME sysTime = { 0 };
	SYSTEMTIME fileSysTime = { 0 };
	FILETIME fileTime = { 0 };
	LARGE_INTEGER FileSize = { };

	HANDLE hHeader = nullptr;
	hHeader = CreateFileA(Path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, NULL, NULL);
	GetSystemTime(&sysTime);

	if (hHeader == INVALID_HANDLE_VALUE)
	{
		return true;
	}

	if (GetFileSizeEx(hHeader, &FileSize) == false)
	{
		return true;
	}
	if ((FileSize.QuadPart == 0) || (FileSize.QuadPart < SDL_DATABASE_MAX_SIZE))
	{

		return true;
	}

	if (GetFileTime(hHeader, 0, 0, &fileTime) == false)
	{
		return true;
	}

	if (CloseHandle(hHeader) == false)
	{
		return true;
	}
	if (FileTimeToSystemTime(&fileTime, &fileSysTime) == false)
	{
		return true;
	}

	if (fileSysTime.wDay != sysTime.wDay)
	{
		return true;
	}
#endif
	return true;
}
bool File::CreateHeaderFromStringA(std::string Path, std::string& Raw_SDL_DB)
{
	std::string HeaderData;
#ifdef WINDOWS
	HeaderData = " \t// ";
	SYSTEMTIME sysTime = { };
	FILETIME fileSysTime = { };
	GetSystemTime(&sysTime);

	HeaderData.append(std::to_string(sysTime.wYear));
	HeaderData.append("-");
	if (sysTime.wMonth < 10)
	{
		HeaderData.append("0");
	}
	HeaderData.append(std::to_string(sysTime.wMonth));
	HeaderData.append("-");
	if (sysTime.wDay < 10)
	{
		HeaderData.append("0");
	}
	HeaderData.append(std::to_string(sysTime.wDay));

	HeaderData.append(" ");
	// You can change the language translation here.
	switch (sysTime.wDayOfWeek)
	{
	case 0:
	{
		HeaderData.append("Sunday");
		break;
	}
	case 1:
	{
		HeaderData.append("Monday");
		break;
	}
	case 2:
	{
		HeaderData.append("Tuesday");
		break;
	}
	case 3:
	{
		HeaderData.append("Wednesday");
		break;
	}
	case 4:
	{
		HeaderData.append("Thursday");
		break;
	}
	case 5:
	{
		HeaderData.append("Friday");
		break;
	}
	case 6:
	{
		HeaderData.append("Saturday");
		break;
	}
	default:
	{
#ifdef UNICODE
		std::wstring DebugString = L"Unknown day of the week: ";
		DebugString.append(std::to_wstring(sysTime.wDayOfWeek));
#else
		std::string DebugString = "Unknown day of the week: ";
		DebugString.append(std::to_string(sysTime.wDayOfWeek));
#endif // UNICODE
		OutputDebugString(DebugString.c_str());
		break;
	}
	}
#endif // WIN32
	HeaderData.append(" \n#pragma once\n#include \"pch.h\"\nconst GAMEPAD_DATA gamepadDB[] = {\n");
	size_t StartPos = 0;
	size_t Pos = 0;
	while (Pos < Raw_SDL_DB.size()) {
		Pos = Raw_SDL_DB.find_first_of('\n', Pos);
		if (Pos == std::string::npos)
		{
			return FALSE;
		}
		else if (Pos == StartPos)
		{
			Pos++;
		}
		else {
			std::string ControllerData = Raw_SDL_DB.substr(StartPos, (Pos - StartPos));
			size_t ConPos = 0;
			size_t StartConPos = 0;
			std::string gamepadName;
			std::string Vid;
			std::string Pid;
			GAMEPAD_DATA gamepadData = { };
			while (ConPos < ControllerData.size()) {
				ConPos = ControllerData.find_first_of(',', ConPos);
				if (ConPos == std::string::npos)
				{
					break;
				}
				else if (ConPos == StartConPos)
				{
					ConPos++;
				}
				else
				{
					std::string SubData = ControllerData.substr(StartConPos, (ConPos - StartConPos));
					size_t ConPosButton = SubData.find_first_of(':');
					if (ConPosButton == std::string::npos && SubData.size() == 32 && StartConPos == 0)//Should be sdl vid & pid encoded string
					{
						if (SubData[1] == '5')
						{
							Pid = SubData.substr(PID_POS, 4);
						}
						else if (SubData[1] == '3')
						{
							Pid = SubData.substr(18, 2) + SubData.substr(16, 2);
						}
						else
						{
							Vid = "";
							Pid = "";
						}
						Vid = SubData.substr(VID_POS_2, 2) + SubData.substr(VID_POS_1, 2);
					}
					else if (ConPosButton == std::string::npos && ConPos != ControllerData.size())//Should be the gamepad name
					{
						if (Vid == Pid)
						{
							gamepadName = "";
						}
						else
						{
							gamepadName = SubData;
						}
					}
					else
					{
						std::string DataType = SubData.substr(0, ConPosButton);
						std::string Mapping = SubData.substr(ConPosButton + 1);
						if (Mapping.find_first_of('~') != std::string::npos)
						{
							Mapping = Mapping.substr(1, Mapping.find_first_of('~') - 1);
						}
						else if (Mapping[1] == '0' && Mapping.length() == 3)
						{
							Mapping = "b" + Mapping.substr(2, Mapping.find_first_of('0') + 1);
						}


						if (DataType == "a")
						{
							gamepadData.bButtonA = Mapping;
						}
						else if (DataType == "b")
						{
							gamepadData.bButtonB = Mapping;

						}
						else if (DataType == "x")
						{
							gamepadData.bButtonX = Mapping;
						}
						else if (DataType == "y")
						{
							gamepadData.bButtonY = Mapping;
						}
						else if (DataType == "dpdown")
						{
							gamepadData.bButtonDpadDown = Mapping;
						}
						else if (DataType == "dpleft")
						{
							gamepadData.bButtonDpadLeft = Mapping;
						}
						else if (DataType == "dpright")
						{
							gamepadData.bButtonDpadRight = Mapping;
						}
						else if (DataType == "dpup")
						{
							gamepadData.bButtonDpadUp = Mapping;
						}
						else if (DataType == "guide")
						{
							gamepadData.bButtonGuide = Mapping;
						}
						else if (DataType == "leftshoulder")
						{
							gamepadData.bButtonLeftShoulder = Mapping;
						}
						else if (DataType == "leftstick")
						{
							gamepadData.bButtonLeftStick = Mapping;
						}
						else if (DataType == "lefttrigger")
						{
							gamepadData.bLeftTrigger = Mapping;
						}
						else if (DataType == "leftx" || DataType == "+leftx" || DataType == "-leftx")
						{
							gamepadData.sThumbLX = Mapping;
						}
						else if (DataType == "lefty" || DataType == "+lefty" || DataType == "-lefty")
						{
							gamepadData.sThumbLY = Mapping;
						}
						else if (DataType == "rightshoulder")
						{
							gamepadData.bButtonRightShoulder = Mapping;
						}
						else if (DataType == "rightstick")
						{
							gamepadData.bButtonRightStick = Mapping;
						}
						else if (DataType == "righttrigger")
						{
							gamepadData.bRightTrigger = Mapping;
						}
						else if (DataType == "rightx" || DataType == "+rightx" || DataType == "-rightx")
						{
							gamepadData.sThumbRX = Mapping;
						}
						else if (DataType == "righty" || DataType == "+righty" || DataType == "-righty")
						{
							gamepadData.sThumbRY = Mapping;
						}
						else if (DataType == "start")
						{
							gamepadData.bButtonStart = Mapping;
						}
						else if (DataType == "back")
						{
							gamepadData.bButtonBack = Mapping;
						}
						else if (DataType == "platform")
						{
							if (Mapping == "Android" || Mapping == "iOS")
							{
								goto Done;
							}
							break;
						}
#ifdef _DEBUG
						else
						{
#ifdef WINDOWS
							OutputDebugStringW(L"Unknown data type: \n\"");
							OutputDebugStringA(DataType.c_str());
							OutputDebugStringW(L"\"\n");
#endif // WIN32
						}
#endif
					}

				}
				StartConPos = ConPos;
			}
			if (gamepadName.size() && Vid != "" && Pid != "")
			{
				HeaderData.append(" { 0x");
				HeaderData.append(Vid);
				HeaderData.append(", 0x");
				HeaderData.append(Pid);
				HeaderData.append(", L\"");

				HeaderData.append(gamepadName);
				HeaderData.append("\", ");
				if (gamepadData.bLeftTrigger.find_first_of('a') == std::string::npos)
				{
					if (gamepadData.bLeftTrigger.find_first_of('b') != std::string::npos)
					{
						HeaderData.append(gamepadData.bLeftTrigger.substr(gamepadData.bLeftTrigger.find_first_of('b') + 1));
						HeaderData.append(", ");
					}
					else
					{
						HeaderData.append("0xFF, ");
					}
				}
				else
				{
					HeaderData.append(gamepadData.bLeftTrigger.substr(gamepadData.bLeftTrigger.find_first_of('a') + 1));
					HeaderData.append(", ");
				}

				if (gamepadData.bRightTrigger.find_first_of('a') == std::string::npos)
				{
					if (gamepadData.bRightTrigger.find_first_of('b') != std::string::npos)
					{
						HeaderData.append(gamepadData.bRightTrigger.substr(gamepadData.bRightTrigger.find_first_of('b') + 1));
						HeaderData.append(", ");
					}
					else
					{
						HeaderData.append("0xFF, ");
					}
				}
				else
				{
					HeaderData.append(gamepadData.bRightTrigger.substr(gamepadData.bRightTrigger.find_first_of('a') + 1));
					HeaderData.append(", ");
				}

				if (gamepadData.bRightTrigger.find_first_of('a') == std::string::npos || gamepadData.bLeftTrigger.find_first_of('a') == std::string::npos)
				{
					if (gamepadData.bRightTrigger.find_first_of('b') != std::string::npos || gamepadData.bLeftTrigger.find_first_of('b') != std::string::npos)
					{
						HeaderData.append("L\"b\", ");
					}
					else
					{
						HeaderData.append("L\"\", ");
					}
				}
				else
				{
					HeaderData.append("L\"a\", ");
				}

				if (gamepadData.sThumbLX.find_first_of('a') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.sThumbLX.substr(gamepadData.sThumbLX.find_first_of('a') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.sThumbLY.find_first_of('a') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.sThumbLY.substr(gamepadData.sThumbLY.find_first_of('a') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.sThumbRX.find_first_of('a') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.sThumbRX.substr(gamepadData.sThumbRX.find_first_of('a') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.sThumbRY.find_first_of('a') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.sThumbRY.substr(gamepadData.sThumbRY.find_first_of('a') + 1));
					HeaderData.append(", ");
				}

				if (gamepadData.bButtonA.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonA.substr(gamepadData.bButtonA.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonB.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonB.substr(gamepadData.bButtonB.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonX.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonX.substr(gamepadData.bButtonX.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonY.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonY.substr(gamepadData.bButtonY.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonBack.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonBack.substr(gamepadData.bButtonBack.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonGuide.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonGuide.substr(gamepadData.bButtonGuide.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonStart.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonStart.substr(gamepadData.bButtonStart.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonLeftStick.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonLeftStick.substr(gamepadData.bButtonLeftStick.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonRightStick.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonRightStick.substr(gamepadData.bButtonRightStick.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonLeftShoulder.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonLeftShoulder.substr(gamepadData.bButtonLeftShoulder.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonRightShoulder.find_first_of('b') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonRightShoulder.substr(gamepadData.bButtonRightShoulder.find_first_of('b') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonDpadUp.find_first_of('.') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonDpadUp.substr(gamepadData.bButtonDpadUp.find_first_of('.') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonDpadDown.find_first_of('.') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonDpadDown.substr(gamepadData.bButtonDpadDown.find_first_of('.') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonDpadLeft.find_first_of('.') == std::string::npos)
				{
					HeaderData.append("0xFF, ");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonDpadLeft.substr(gamepadData.bButtonDpadLeft.find_first_of('.') + 1));
					HeaderData.append(", ");
				}
				if (gamepadData.bButtonDpadRight.find_first_of('.') == std::string::npos)
				{
					HeaderData.append("0xFF },");
				}
				else
				{
					HeaderData.append(gamepadData.bButtonDpadRight.substr(gamepadData.bButtonDpadRight.find_first_of('.') + 1));
					HeaderData.append(" },");
				}
				HeaderData.append("\n");
			}
		}
		StartPos = Pos;
	}
Done:
	HeaderData.append(" };");
	HANDLE hHeader = CreateFileA(Path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);
	if (hHeader == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringW(L"Failed to create header file! \n");
		return FALSE;
	}
	if (WriteFile(hHeader, HeaderData.data(), (DWORD)HeaderData.length(), NULL, NULL) == FALSE)
	{
		CloseHandle(hHeader);
		OutputDebugStringW(L"Failed to write header file! \n");
		return FALSE;
	}
	CloseHandle(hHeader);
	return TRUE;
}
#endif
#pragma warning( pop )