#pragma once
namespace HTTP
{
#ifdef WINDOWS
#ifdef UNICODE
bool DownloadSDLFileW(std::wstring SDL_HOST, std::wstring SDL_PATH, std::string& SDL_Database);
#else
bool DownloadSDLFileA(std::string SDL_HOST, std::string SDL_PATH, std::string& SDL_Database);
#endif
#endif
};