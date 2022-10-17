#pragma once
typedef enum GameInputKind
{
	GameInputKindUnknown = 0x00000000,
	GameInputKindRawDeviceReport = 0x00000001,
	GameInputKindController = 0x00000002,
	GameInputKindKeyboard = 0x00000004,
	GameInputKindMouse = 0x00000008,
	GameInputKindTouch = 0x00000100,
	GameInputKindMotion = 0x00001000,
	GameInputKindArcadeStick = 0x00010000,
	GameInputKindFlightStick = 0x00020000,
	GameInputKindGamepad = 0x00040000,
	GameInputKindRacingWheel = 0x00080000,
	GameInputKindUiNavigation = 0x01000000,
	GameInputKindAny = 0x0FFFFFFF
} GameInputKind;
typedef enum GameInputGamepadButtons
{
	GameInputGamepadNone = 0x00000000,
	GameInputGamepadMenu = 0x00000001,
	GameInputGamepadView = 0x00000002,
	GameInputGamepadA = 0x00000004,
	GameInputGamepadB = 0x00000008,
	GameInputGamepadX = 0x00000010,
	GameInputGamepadY = 0x00000020,
	GameInputGamepadDPadUp = 0x00000040,
	GameInputGamepadDPadDown = 0x00000080,
	GameInputGamepadDPadLeft = 0x00000100,
	GameInputGamepadDPadRight = 0x00000200,
	GameInputGamepadLeftShoulder = 0x00000400,
	GameInputGamepadRightShoulder = 0x00000800,
	GameInputGamepadLeftThumbstick = 0x00001000,
	GameInputGamepadRightThumbstick = 0x00002000
} GameInputGamepadButtons;
typedef struct GameInputGamepadState {
	GameInputGamepadButtons buttons;
	float leftTrigger;
	float rightTrigger;
	float leftThumbstickX;
	float leftThumbstickY;
	float rightThumbstickX;
	float rightThumbstickY;
} GameInputGamepadState;
typedef enum GameInputSwitchPosition
{
	GameInputSwitchCenter = 0,
	GameInputSwitchUp = 1,
	GameInputSwitchUpRight = 2,
	GameInputSwitchRight = 3,
	GameInputSwitchDownRight = 4,
	GameInputSwitchDown = 5,
	GameInputSwitchDownLeft = 6,
	GameInputSwitchLeft = 7,
	GameInputSwitchUpLeft = 8
} GameInputSwitchPosition;
typedef enum GameInputFlightStickButtons
{
	GameInputFlightStickNone = 0x00000000,
	GameInputFlightStickMenu = 0x00000001,
	GameInputFlightStickView = 0x00000002,
	GameInputFlightStickFirePrimary = 0x00000004,
	GameInputFlightStickFireSecondary = 0x00000008
} GameInputFlightStickButtons;
typedef struct GameInputFlightStickState {
	GameInputFlightStickButtons buttons;
	GameInputSwitchPosition hatSwitch;
	float roll;
	float pitch;
	float yaw;
	float throttle;
} GameInputFlightStickState;
typedef enum GameInputArcadeStickButtons
{
	GameInputArcadeStickNone = 0x00000000,
	GameInputArcadeStickMenu = 0x00000001,
	GameInputArcadeStickView = 0x00000002,
	GameInputArcadeStickUp = 0x00000004,
	GameInputArcadeStickDown = 0x00000008,
	GameInputArcadeStickLeft = 0x00000010,
	GameInputArcadeStickRight = 0x00000020,
	GameInputArcadeStickAction1 = 0x00000040,
	GameInputArcadeStickAction2 = 0x00000080,
	GameInputArcadeStickAction3 = 0x00000100,
	GameInputArcadeStickAction4 = 0x00000200,
	GameInputArcadeStickAction5 = 0x00000400,
	GameInputArcadeStickAction6 = 0x00000800,
	GameInputArcadeStickSpecial1 = 0x00001000,
	GameInputArcadeStickSpecial2 = 0x00002000
} GameInputArcadeStickButtons;
typedef struct GameInputArcadeStickState {
	GameInputArcadeStickButtons buttons;
} GameInputArcadeStickState;
typedef struct GameInputKeyState {
	uint32_t scanCode;
	uint32_t codePoint;
	uint8_t virtualKey;
	bool isDeadKey;
} GameInputKeyState;
typedef enum GameInputMotionAccuracy
{
	GameInputMotionAccuracyUnknown = -1,
	GameInputMotionUnavailable = 0,
	GameInputMotionUnreliable = 1,
	GameInputMotionApproximate = 2,
	GameInputMotionAccurate = 3
} GameInputMotionAccuracy;
typedef struct GameInputMotionState {
	float accelerationX;
	float accelerationY;
	float accelerationZ;
	float angularVelocityX;
	float angularVelocityY;
	float angularVelocityZ;
	float magneticFieldX;
	float magneticFieldY;
	float magneticFieldZ;
	float orientationW;
	float orientationX;
	float orientationY;
	float orientationZ;
	GameInputMotionAccuracy magnetometerAccuracy;
	GameInputMotionAccuracy orientationAccuracy;
} GameInputMotionState;
typedef enum GameInputMouseButtons
{
	GameInputMouseNone = 0x00000000,
	GameInputMouseLeftButton = 0x00000001,
	GameInputMouseRightButton = 0x00000002,
	GameInputMouseMiddleButton = 0x00000004,
	GameInputMouseButton4 = 0x00000008,
	GameInputMouseButton5 = 0x00000010,
	GameInputMouseWheelTiltLeft = 0x00000020,
	GameInputMouseWheelTiltRight = 0x00000040
} GameInputMouseButtons;
typedef struct GameInputMouseState {
	GameInputMouseButtons buttons;
	int64_t positionX;
	int64_t positionY;
	int64_t wheelX;
	int64_t wheelY;
} GameInputMouseState;
typedef enum GameInputRacingWheelButtons
{
	GameInputRacingWheelNone = 0x00000000,
	GameInputRacingWheelMenu = 0x00000001,
	GameInputRacingWheelView = 0x00000002,
	GameInputRacingWheelPreviousGear = 0x00000004,
	GameInputRacingWheelNextGear = 0x00000008,
	GameInputRacingWheelDpadUp = 0x00000010,
	GameInputRacingWheelDpadDown = 0x00000020,
	GameInputRacingWheelDpadLeft = 0x00000040,
	GameInputRacingWheelDpadRight = 0x00000080
} GameInputRacingWheelButtons;
typedef struct GameInputRacingWheelState {
	GameInputRacingWheelButtons buttons;
	int32_t patternShifterGear;
	float wheel;
	float throttle;
	float brake;
	float clutch;
	float handbrake;
} GameInputRacingWheelState;
typedef struct GameInputTouchState {
	uint64_t touchId;
	uint32_t sensorIndex;
	float positionX;
	float positionY;
	float pressure;
	float proximity;
	float contactRectTop;
	float contactRectLeft;
	float contactRectRight;
	float contactRectBottom;
} GameInputTouchState;
typedef enum GameInputUiNavigationButtons
{
	GameInputUiNavigationNone = 0x00000000,
	GameInputUiNavigationMenu = 0x00000001,
	GameInputUiNavigationView = 0x00000002,
	GameInputUiNavigationAccept = 0x00000004,
	GameInputUiNavigationCancel = 0x00000008,
	GameInputUiNavigationUp = 0x00000010,
	GameInputUiNavigationDown = 0x00000020,
	GameInputUiNavigationLeft = 0x00000040,
	GameInputUiNavigationRight = 0x00000080,
	GameInputUiNavigationContext1 = 0x00000100,
	GameInputUiNavigationContext2 = 0x00000200,
	GameInputUiNavigationContext3 = 0x00000400,
	GameInputUiNavigationContext4 = 0x00000800,
	GameInputUiNavigationPageUp = 0x00001000,
	GameInputUiNavigationPageDown = 0x00002000,
	GameInputUiNavigationPageLeft = 0x00004000,
	GameInputUiNavigationPageRight = 0x00008000,
	GameInputUiNavigationScrollUp = 0x00010000,
	GameInputUiNavigationScrollDown = 0x00020000,
	GameInputUiNavigationScrollLeft = 0x00040000,
	GameInputUiNavigationScrollRight = 0x00080000
} GameInputUiNavigationButtons;
typedef struct GameInputUiNavigationState {
	GameInputUiNavigationButtons buttons;
} GameInputUiNavigationState;

struct IGameInputRawDeviceReport : public IUnknown
{

};
struct IGameInputDevice : public IUnknown
{};
struct IGameInputDispatcher : public IUnknown
{

};
struct IGameInputReading : public IUnknown
{
	BEGIN_INTERFACE
	virtual bool GetArcadeStickState(
		GameInputArcadeStickState* state
	) = 0;
	virtual uint32_t GetControllerAxisCount(
	) = 0;
	virtual uint32_t GetControllerAxisState(
		uint32_t stateArrayCount,
		float* stateArray
	) = 0;
	virtual uint32_t GetControllerButtonCount(
	) = 0;
	virtual uint32_t GetControllerButtonState(
		uint32_t stateArrayCount,
		bool* stateArray
	) = 0;
	virtual uint32_t GetControllerSwitchCount(
	) = 0;
	virtual uint32_t GetControllerSwitchState(
		uint32_t stateArrayCount,
		GameInputSwitchPosition* stateArray
	) = 0;
	virtual void GetDevice(
		IGameInputDevice** device
	) = 0;
	virtual bool GetFlightStickState(
		GameInputFlightStickState* state
	) = 0;
	virtual bool GetGamepadState(
		GameInputGamepadState* state
	) = 0;
	virtual GameInputKind GetInputKind(
	) = 0;
	virtual uint32_t GetKeyCount(
	) = 0;
	virtual uint32_t GetKeyState(
		uint32_t stateArrayCount,
		GameInputKeyState* stateArray
	) = 0;
	virtual bool GetMotionState(
		GameInputMotionState* state
	) = 0;
	virtual bool GetMouseState(
		GameInputMouseState* state
	) = 0;
	virtual bool GetRacingWheelState(
		GameInputRacingWheelState* state
	) = 0;
	virtual bool GetRawReport(
		IGameInputRawDeviceReport** report
	) = 0;
	virtual uint64_t GetSequenceNumber(
		GameInputKind inputKind
	) = 0;
	virtual uint64_t GetTimestamp(
	) = 0;
	virtual uint32_t GetTouchCount(
	) = 0;
	virtual uint32_t GetTouchState(
		uint32_t stateArrayCount,
		GameInputTouchState* stateArray
	) = 0;
	virtual bool GetUiNavigationState(
		GameInputUiNavigationState* state
	) = 0;
	END_INTERFACE

};
struct IGameInput : public IUnknown
{
	BEGIN_INTERFACE
	virtual HRESULT STDMETHODCALLTYPE CreateAggregateDevice(
		GameInputKind inputKind,
		IGameInputDevice** device
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateDispatcher(
		IGameInputDispatcher** dispatcher
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE EnableOemDeviceSupport(
		uint16_t vendorId,
		uint16_t productId,
		uint8_t interfaceNumber,
		uint8_t collectionNumber
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE FindDeviceFromObject(
		IUnknown* value,
		IGameInputDevice** device
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE FindDeviceFromPlatformHandle(
		HANDLE value,
		IGameInputDevice** device
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE FindDeviceFromPlatformString(
		LPCWSTR value,
		IGameInputDevice** device
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentReading(
		GameInputKind inputKind,
		IGameInputDevice* device,
		IGameInputReading** reading
	) = 0;
	virtual uint64_t STDMETHODCALLTYPE GetCurrentTimestamp(
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDeviceFromId(
		const APP_LOCAL_DEVICE_ID* value,
		IGameInputDevice** device
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetNextReading(
		IGameInputReading* referenceReading,
		GameInputKind inputKind,
		IGameInputDevice* device,
		IGameInputReading** reading
	) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetPreviousReading(
		IGameInputReading* referenceReading,
		GameInputKind inputKind,
		IGameInputDevice* device,
		IGameInputReading** reading
	) = 0;
	END_INTERFACE
};
namespace GameInput
{
	DWORD GetState(GenericInputController& controller, GENERIC_INPUT_STATE* pState);
}