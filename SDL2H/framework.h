#pragma once
// Change this macro to define the max size in bytes for the sdl controller database.
#define SDL_DATABASE_MAX_SIZE 2048

#ifdef WINDOWS

#pragma warning( push )
#pragma warning( disable : 5039)
#pragma warning( disable : 4668)
#pragma warning( disable : 4820)
#pragma warning( disable : 4365)
#pragma warning( disable : 4067)
#include <Windows.h>
#pragma warning( pop )

#pragma warning( push )
#pragma warning( disable : 4514)
#pragma warning( disable : 4820)
#pragma warning( disable : 4365)
#include <shlwapi.h>
#pragma warning( pop )

#pragma warning( push )
#pragma warning( disable : 4820)
#pragma warning( disable : 4365)
#pragma warning( disable : 4365)
#include <winhttp.h>
#pragma warning( pop )
#endif

#pragma warning( push )
#pragma warning( disable : 4365)
#pragma warning( disable : 4710)
#pragma warning( disable : 4711)

#include <string>
#include <vector>
#include <sstream>
#pragma warning( pop )
#include "string.h"
#include "user_agent.h"
#ifdef UNICODE
#define ShouldHeaderFileBeRebuilt ShouldHeaderFileBeRebuiltW
#define CreateHeaderFromString CreateHeaderFromStringW
#else
#define ShouldHeaderFileBeRebuilt ShouldHeaderFileBeRebuiltA
#define CreateHeaderFromString CreateHeaderFromStringA
#endif // UNICODE