# GenericInput
# GenericInput development has been restarted.
GenericInput is a HID controller input library written in C++ that aims to clone proprietary controller libraries. macOS is not supported 
# Dependencies
GameInput Redistributable
# Initializing GenericInput
Initialize GenericInput by passing a Window Handle and a boolean to tell GenericInput whether or not to close your application upon failure.
```c++
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   
   if (GenericInputInit(hWnd, FALSE) == ERROR_GEN_FAILURE)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
```

# Windows Procedure

Add GenericInputDeviceChange to your Windows procedure:
```c++
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GenericInputDeviceChange(hWnd, message, wParam, lParam);
    
    return 0;
}
```

GenericInput will send the registered window a message, if it detects a controller connection event:
```c++
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CONTROLLER_CONNECTED:
    {
      // A controller was connected and wParam is the index of the controller that was connected.
    }
    case WM_CONTROLLER_DISCONNECTED:
    {
      // A controller was disconnected and wParam is the index of the controller that was disconnected.
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
```

# Documentation & Usage

**GENERIC_INPUT_STATE is intercompatible with XINPUT_STATE**

Call GenericInputGetState with a controller index to get the state of that controller.
```c++
void InputManager::GamepadUpdate(DWORD dwUserIndex)
{
    GENERIC_INPUT_STATE gamepadState = { 0 };
    if (GenericInputGetState(&gamepadState, dwUserIndex) == ERROR_SUCCESS)
    {
            if (state.Gamepad.wButtons & CONTROLLER_BUTTON_A)
            {
                // Do something
            }
    }
}
```

To get the layout of the current controller, call GenericInputGetLayout with the an index of the desired controller.
```c++
void InputManager::GamepadUpdate(DWORD dwUserIndex)
{
    GENERIC_INPUT_STATE gamepadState = { 0 };
    if (GenericInputGetState(&gamepadState, dwUserIndex) == ERROR_SUCCESS)
    {
         controllerType = GenericInputGetLayout(dwUserIndex);
         switch (controllerType)
         {
         case NC:// Not connected
         {
            // Change the gui button prompts
            break;
         }
         case XInput:// Xbox Controller
         {
            // Change the gui button prompts
            break;
         }
         case DS: // PlayStation Controller
         {
            // Change the gui button prompts
            break;
         }
         case NT: // Nintendo Controller
         {
            // Change the gui button prompts
            break;
         }
         // Found in the game controller database at compile time. 
         case SDL: // Generic Controller
         {
            // Change the gui button prompts
            break;
         }
      }
}
```
