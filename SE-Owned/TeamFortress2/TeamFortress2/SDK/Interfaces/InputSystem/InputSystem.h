#pragma once
#include <cstdint>
#if !defined( _X360 )
#define INVALID_USER_ID		-1
#else
#define INVALID_USER_ID		XBX_INVALID_USER_ID
#endif
#include "../Input/Input.h"

struct InputEvent_t
{
	int m_nType;				// Type of the event (see InputEventType_t)
	int m_nTick;				// Tick on which the event occurred
	int m_nData;				// Generic 32-bit data, what it contains depends on the event
	int m_nData2;				// Generic 32-bit data, what it contains depends on the event
	int m_nData3;				// Generic 32-bit data, what it contains depends on the event
};

class ISteamController;

enum ESteamControllerPad;

enum EControllerSource
{
	k_EControllerSource_None,
	k_EControllerSource_LeftTrackpad,
	k_EControllerSource_RightTrackpad,
	k_EControllerSource_Joystick,
	k_EControllerSource_ABXY,
	k_EControllerSource_Switch,
	k_EControllerSource_LeftTrigger,
	k_EControllerSource_RightTrigger,
	k_EControllerSource_Gyro
};

enum EControllerSourceMode
{
	k_EControllerSourceMode_None,
	k_EControllerSourceMode_Dpad,
	k_EControllerSourceMode_Buttons,
	k_EControllerSourceMode_FourButtons,
	k_EControllerSourceMode_AbsoluteMouse,
	k_EControllerSourceMode_RelativeMouse,
	k_EControllerSourceMode_JoystickMove,
	k_EControllerSourceMode_JoystickCamera,
	k_EControllerSourceMode_ScrollWheel,
	k_EControllerSourceMode_Trigger,
	k_EControllerSourceMode_TouchMenu
};

enum EControllerActionOrigin
{
	k_EControllerActionOrigin_None,
	k_EControllerActionOrigin_A,
	k_EControllerActionOrigin_B,
	k_EControllerActionOrigin_X,
	k_EControllerActionOrigin_Y,
	k_EControllerActionOrigin_LeftBumper,
	k_EControllerActionOrigin_RightBumper,
	k_EControllerActionOrigin_LeftGrip,
	k_EControllerActionOrigin_RightGrip,
	k_EControllerActionOrigin_Start,
	k_EControllerActionOrigin_Back,
	k_EControllerActionOrigin_LeftPad_Touch,
	k_EControllerActionOrigin_LeftPad_Swipe,
	k_EControllerActionOrigin_LeftPad_Click,
	k_EControllerActionOrigin_LeftPad_DPadNorth,
	k_EControllerActionOrigin_LeftPad_DPadSouth,
	k_EControllerActionOrigin_LeftPad_DPadWest,
	k_EControllerActionOrigin_LeftPad_DPadEast,
	k_EControllerActionOrigin_RightPad_Touch,
	k_EControllerActionOrigin_RightPad_Swipe,
	k_EControllerActionOrigin_RightPad_Click,
	k_EControllerActionOrigin_RightPad_DPadNorth,
	k_EControllerActionOrigin_RightPad_DPadSouth,
	k_EControllerActionOrigin_RightPad_DPadWest,
	k_EControllerActionOrigin_RightPad_DPadEast,
	k_EControllerActionOrigin_LeftTrigger_Pull,
	k_EControllerActionOrigin_LeftTrigger_Click,
	k_EControllerActionOrigin_RightTrigger_Pull,
	k_EControllerActionOrigin_RightTrigger_Click,
	k_EControllerActionOrigin_LeftStick_Move,
	k_EControllerActionOrigin_LeftStick_Click,
	k_EControllerActionOrigin_LeftStick_DPadNorth,
	k_EControllerActionOrigin_LeftStick_DPadSouth,
	k_EControllerActionOrigin_LeftStick_DPadWest,
	k_EControllerActionOrigin_LeftStick_DPadEast,
	k_EControllerActionOrigin_Gyro_Move,
	k_EControllerActionOrigin_Gyro_Pitch,
	k_EControllerActionOrigin_Gyro_Yaw,
	k_EControllerActionOrigin_Gyro_Roll,

	k_EControllerActionOrigin_Count
};

// ControllerHandle_t is used to refer to a specific controller.
// This handle will consistently identify a controller, even if it is disconnected and re-connected
typedef uint64_t ControllerHandle_t;


typedef uint64_t ControllerActionSetHandle_t;
typedef uint64_t ControllerDigitalActionHandle_t;
typedef uint64_t ControllerAnalogActionHandle_t;

#define JOYSTICK_AXIS_INTERNAL( _joystick, _axis ) ( JOYSTICK_FIRST_AXIS + ((_joystick) * MAX_JOYSTICK_AXES) + (_axis) )
#define JOYSTICK_AXIS( _joystick, _axis ) ( (AnalogCode_t)JOYSTICK_AXIS_INTERNAL( _joystick, _axis ) )

enum
{
	MAX_JOYSTICKS = 1,
	MOUSE_BUTTON_COUNT = 5,
	MAX_NOVINT_DEVICES = 2,
};

enum JoystickAxis_t
{
	JOY_AXIS_X = 0,
	JOY_AXIS_Y,
	JOY_AXIS_Z,
	JOY_AXIS_R,
	JOY_AXIS_U,
	JOY_AXIS_V,
	MAX_JOYSTICK_AXES,
};

enum AnalogCode_t
{
	ANALOG_CODE_INVALID = -1,
	MOUSE_X = 0,
	MOUSE_Y,
	MOUSE_XY,		// Invoked when either x or y changes
	MOUSE_WHEEL,

	JOYSTICK_FIRST_AXIS,
	JOYSTICK_LAST_AXIS = JOYSTICK_AXIS_INTERNAL(MAX_JOYSTICKS - 1, MAX_JOYSTICK_AXES - 1),

	ANALOG_CODE_LAST,
};

class IInputSystem
{
public:
	virtual void AttachToWindow(void* hWnd) = 0;
	virtual void DetachFromWindow() = 0;
	virtual void EnableInput(bool bEnable) = 0;
	virtual void EnableMessagePump(bool bEnable) = 0;
	virtual void PollInputState() = 0;
	virtual int GetPollTick() const = 0;
	virtual bool IsButtonDown(ButtonCode_t code) const = 0;
	virtual int GetButtonPressedTick(ButtonCode_t code) const = 0;
	virtual int GetButtonReleasedTick(ButtonCode_t code) const = 0;
	virtual int GetAnalogValue(AnalogCode_t code) const = 0;
	virtual int GetAnalogDelta(AnalogCode_t code) const = 0;
	virtual int GetEventCount() const = 0;
	virtual const InputEvent_t* GetEventData() const = 0;
	virtual void PostUserEvent(const InputEvent_t& event) = 0;
	virtual int GetJoystickCount() const = 0;
	virtual void EnableJoystickInput(int nJoystick, bool bEnable) = 0;
	virtual void EnableJoystickDiagonalPOV(int nJoystick, bool bEnable) = 0;
	virtual void SampleDevices(void) = 0;
	virtual void SetRumble(float fLeftMotor, float fRightMotor, int userId = INVALID_USER_ID) = 0;
	virtual void StopRumble(void) = 0;
	virtual void ResetInputState() = 0;
	virtual void SetPrimaryUserId(int userId) = 0;
	virtual const char* ButtonCodeToString(ButtonCode_t code) const = 0;
	virtual const char* AnalogCodeToString(AnalogCode_t code) const = 0;
	virtual ButtonCode_t StringToButtonCode(const char* pString) const = 0;
	virtual AnalogCode_t StringToAnalogCode(const char* pString) const = 0;
	virtual void SleepUntilInput(int nMaxSleepTimeMS = -1) = 0;
	virtual ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey) const = 0;
	virtual int ButtonCodeToVirtualKey(ButtonCode_t code) const = 0;
	virtual ButtonCode_t ScanCodeToButtonCode(int lParam) const = 0;
	virtual int GetPollCount() const = 0;
	virtual void SetCursorPosition(int x, int y) = 0;
	virtual void* GetHapticsInterfaceAddress() const = 0;
	virtual void SetNovintPure(bool bPure) = 0;
	virtual bool GetRawMouseAccumulators(int& accumX, int& accumY) = 0;
	virtual void SetConsoleTextMode(bool bConsoleTextMode) = 0;
	virtual ISteamController* SteamControllerInterface() = 0;
	virtual uint32_t GetNumSteamControllersConnected() = 0;
	virtual bool IsSteamControllerActive() = 0;
	virtual bool IsSteamControllerConnected() = 0;
	virtual int GetSteamControllerIndexForSlot(int nSlot) = 0;
	virtual bool GetRadialMenuStickValues(int nSlot, float& fX, float& fY) = 0;
	virtual void ActivateSteamControllerActionSetForSlot(uint64_t nSlot, GameActionSet_t eActionSet) = 0;
	virtual ControllerActionSetHandle_t GetActionSetHandle(GameActionSet_t eActionSet) = 0;
	virtual ControllerActionSetHandle_t GetActionSetHandle(const char* szActionSet) = 0;
	virtual EControllerActionOrigin GetSteamControllerActionOrigin(const char* action, GameActionSet_t action_set) = 0;
	virtual EControllerActionOrigin GetSteamControllerActionOrigin(const char* action, ControllerActionSetHandle_t action_set_handle) = 0;
	virtual const wchar_t* GetSteamControllerFontCharacterForActionOrigin(EControllerActionOrigin origin) = 0;
	virtual const wchar_t* GetSteamControllerDescriptionForActionOrigin(EControllerActionOrigin origin) = 0;
	virtual void SetSkipControllerInitialization(bool bSkip) = 0;
	//virtual void EnableInput(bool bEnable) = 0;
	void ResetInputStateVFunc()
	{
		typedef void(__thiscall* OrgFn)(PVOID);
		GetVFunc<OrgFn>(this, 25)(this);
	}
};