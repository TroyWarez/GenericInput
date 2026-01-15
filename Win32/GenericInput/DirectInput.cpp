#include "pch.h"
#include "DirectInput.h"
HRESULT DirectInput::DirectInput8Create(
    HINSTANCE hinst,
    DWORD dwVersion,
    REFIID riidltf,
    LPVOID* ppvOut,
    IUnknown* punkOuter
)
{
    return DI_OK;
}
HRESULT DirectInput::GetdfDIJoystick()
{
    return DI_OK;
}

HRESULT DirectInput::joyConfigChanged()
{
    return DI_OK;
}

HRESULT DirectInput::joyGetDevCapsA()
{
    return DI_OK;
}

HRESULT DirectInput::joyGetDevCapsW()
{
    return DI_OK;
}

HRESULT DirectInput::joyGetNumDevs()
{
    return DI_OK;
}

HRESULT DirectInput::joyGetPos()
{
    return DI_OK;
}

HRESULT DirectInput::joyGetPosEx()
{
    return DI_OK;
}

HRESULT DirectInput::joyReleaseCapture()
{
    return DI_OK;
}

HRESULT DirectInput::joySetCapture()
{
    return DI_OK;
}

HRESULT DirectInput::joyGetThreshold()
{
    return DI_OK;
}

HRESULT DirectInput::joySetThreshold()
{
    return DI_OK;
}
