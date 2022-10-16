#pragma once
class IGameInputReading : public IUnknown
{

};
class IGameInputDispatcher : public IUnknown
{

};
class IGameInputDevice : public IUnknown
{

};
class IGameInput : public IUnknown
{
public:
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

	HRESULT CreateAggregateDevice(
		GameInputKind inputKind,
		IGameInputDevice** device
	) { }
	HRESULT CreateDispatcher(
		IGameInputDispatcher** dispatcher
	) { }
	HRESULT EnableOemDeviceSupport(
		uint16_t vendorId,
		uint16_t productId,
		uint8_t interfaceNumber,
		uint8_t collectionNumber
	) { }
	HRESULT FindDeviceFromObject(
		IUnknown* value,
		IGameInputDevice** device
	) { }
	HRESULT FindDeviceFromPlatformHandle(
		HANDLE value,
		IGameInputDevice** device
	) { }
	HRESULT FindDeviceFromPlatformString(
		LPCWSTR value,
		IGameInputDevice** device
	) { }
	HRESULT GetCurrentReading(
		GameInputKind inputKind,
		IGameInputDevice* device,
		IGameInputReading** reading
	) { }
	uint64_t GetCurrentTimestamp(
	) { }
	HRESULT GetDeviceFromId(
		const APP_LOCAL_DEVICE_ID* value,
		IGameInputDevice** device
	) { }
	HRESULT GetNextReading(
		IGameInputReading* referenceReading,
		GameInputKind inputKind,
		IGameInputDevice* device,
		IGameInputReading** reading
	) { }
	HRESULT GetPreviousReading(
		IGameInputReading* referenceReading,
		GameInputKind inputKind,
		IGameInputDevice* device,
		IGameInputReading** reading
	) { }
};
namespace GameInput
{
	DWORD GetState(GenericInputController& controller, GENERIC_INPUT_STATE* pState);
}