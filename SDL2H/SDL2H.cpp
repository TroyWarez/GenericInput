#pragma warning( push )
#pragma warning( disable : 4710)
#include "framework.h"
#include "http.h"
#include "File.h"
#pragma warning( pop )
#pragma warning( push )
#pragma warning( disable : 5045)
#pragma warning( disable : 4711)
#ifdef WINDOWS

#ifdef UNICODE
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nShowCmd);

	std::string SDL_DB;

	std::string CommandLineA = (LPSTR)lpCmdLine;

	std::wstring CommandLineW = string::ConvertAnsiToWide(CommandLineA);

	std::wstringstream CommandLineArgurments(CommandLineW.c_str());

	std::wstring Argurment;
	std::vector<std::wstring> vCommandLineArgurments;
	while (std::getline(CommandLineArgurments, Argurment, L'-'))
	{
		if (Argurment != L"")
		{
			vCommandLineArgurments.push_back(Argurment);
		}
	}
	if (vCommandLineArgurments.size() != 3)
	{
#ifdef _DEBUG
		MessageBox(NULL, L"Invalid arguments \n", L"Invalid arguments! ", MB_OK | MB_ICONERROR);
#endif // _DEBUG
		return 1;
	}

	if (File::ShouldHeaderFileBeRebuiltW(vCommandLineArgurments.front().c_str()) == true) {
		if (HTTP::DownloadSDLFileW(vCommandLineArgurments[1].c_str(), vCommandLineArgurments[2].c_str(), SDL_DB) == FALSE)
		{
#ifdef _DEBUG
			OutputDebugStringW(L"Failed to download the sdl database string! \n");
#endif // _DEBUG
			return ERROR_INVALID_FUNCTION;
		}
		if (File::CreateHeaderFromStringW(vCommandLineArgurments.front().c_str(), SDL_DB) == false)
		{
#ifdef _DEBUG
			OutputDebugStringW(L"Failed to create the sdl database header file! \n");
#endif // _DEBUG
			return ERROR_INVALID_FUNCTION;
		}
#ifdef _DEBUG
		OutputDebugStringW(L"The sdl database header file was rebuilt successfully. \n");
#endif // _DEBUG
	}
	return ERROR_SUCCESS;
}
#else
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nShowCmd);

	std::string SDL_DB;

	std::string CommandLineA = lpCmdLine;

	std::stringstream CommandLineArgurments(CommandLineA.c_str());

	std::string Argurment;
	std::vector<std::string> vCommandLineArgurments;

	while (std::getline(CommandLineArgurments, Argurment, ' '))
	{
		vCommandLineArgurments.push_back(Argurment);
	}
	if (vCommandLineArgurments.size() != 3)
	{
#ifdef _DEBUG
		MessageBox(NULL, "Invalid argurments \n", "Invalid argurments! ", MB_OK | MB_ICONERROR);
#endif // _DEBUG
		return 1;
	}

	if (File::ShouldHeaderFileBeRebuiltA(vCommandLineArgurments.front().c_str()) == true) {
	if (HTTP::DownloadSDLFileA(vCommandLineArgurments[1].c_str(), vCommandLineArgurments[2].c_str(), SDL_DB) == FALSE)
		{
#ifdef _DEBUG
			OutputDebugStringA("Failed to download the sdl database string! \n");
#endif // _DEBUG
			return ERROR_INVALID_FUNCTION;
		}
		if (File::CreateHeaderFromStringA(vCommandLineArgurments.front().c_str(), SDL_DB) == false)
		{
#ifdef _DEBUG
			OutputDebugStringA("Failed to create the sdl database header file! \n");
#endif // _DEBUG
			return ERROR_INVALID_FUNCTION;
		}
#ifdef _DEBUG
		OutputDebugStringA("The sdl database header file was rebuilt successfully. \n");
#endif // _DEBUG
	}
	return ERROR_SUCCESS;
}
#endif // UNICODE

#endif // WIN32
#pragma warning( pop )