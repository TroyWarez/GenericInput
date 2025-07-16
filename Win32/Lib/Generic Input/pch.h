// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#pragma warning( push )
#pragma warning( disable : 5039)
#include <Windows.h>
#include <initguid.h>
DEFINE_GUID(GUID_DEVINTERFACE_XINPUT, 0xec87f1e3, 0xc13b, 0x4100, 0xb5, 0xf7, 0x8b, 0x84, 0xd5, 0x42, 0x60, 0xcb);
#define GUID_CLASS_XINPUT GUID_DEVINTERFACE_XINPUT
//{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}
DEFINE_GUID(GUID_DEVINTERFACE_BLUETOOTH, 0xE0CBF06C, 0xCD8B, 0x4647, 0xBB, 0x8A, 0x26, 0x3B, 0x43, 0xF0, 0xF9, 0x74);
#define GUID_CLASS_BLUETOOTH GUID_DEVINTERFACE_BLUETOOTH
#include <Hidclass.h>
#pragma warning( pop )

#endif //PCH_H
