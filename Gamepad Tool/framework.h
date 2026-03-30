// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <d2d1.h>
#include <wrl/client.h>
#include <Xinput.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h> // For ComPtr
#include <vector>

#define WND_HEIGHT 600
#define WND_WIDTH 800