#pragma once
#define DI_OK S_OK
namespace DirectInput
{
    HRESULT DirectInput8Create(
        HINSTANCE hinst,
        DWORD dwVersion,
        REFIID riidltf,
        LPVOID* ppvOut,
        IUnknown* punkOuter
    );
    HRESULT GetdfDIJoystick();//Add func prototypes
    HRESULT joyConfigChanged();
    HRESULT joyGetDevCapsA();
    HRESULT joyGetDevCapsW();
    HRESULT joyGetNumDevs();
    HRESULT joyGetPos();
    HRESULT joyGetPosEx();
    HRESULT joyReleaseCapture();
    HRESULT joySetCapture();
    HRESULT joyGetThreshold();
    HRESULT joySetThreshold();
};