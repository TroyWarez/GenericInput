#pragma once
namespace File
{
#ifdef UNICODE
	bool ShouldHeaderFileBeRebuiltW(std::wstring Path);
	bool CreateHeaderFromStringW(std::wstring Path, std::string& Raw_SDL_DB);
#else
	bool ShouldHeaderFileBeRebuiltA(std::string Path);
	bool CreateHeaderFromStringA(std::string Path, std::string& Raw_SDL_DB);
#endif
};

