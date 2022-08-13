#include "LuaCallbacks.h"
#include "../Wrapper.hpp"

/* Registers a new Lua callback of the given type */
void CLuaCallbacks::Register(const char* type, const char* name, const sol::function& callback)
{
	if (callback.valid())
	{
		Callbacks[type].emplace(name, callback);
	}
}

/* Unregisters an existing callback by type and name */
void CLuaCallbacks::Unregister(const char* type, const char* name)
{
	Callbacks[type].erase(name);
}

void CLuaCallbacks::Reset()
{
	Callbacks.clear();
}

void CLuaCallbacks::HandleError(const sol::protected_function_result& result)
{
	if (!result.valid())
	{
		const sol::error err = result;
		I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, "Callback Error:\n%s\n", err.what());
	}
}

/* Simple callback without any args */
void CLuaCallbacks::ByType(const char* type) {
	for (const auto& [name, callback] : Callbacks[type])
	{
		if (callback.valid())
		{
			HandleError(callback());
		}
	}
}

void CLuaCallbacks::OnCreateMove(CUserCmd* pCmd, bool* pSendPacket) {
	for (const auto& [name, callback] : Callbacks["CreateMove"])
	{
		if (callback.valid())
		{
			HandleError(callback(WUserCmd(pCmd, pSendPacket)));
		}
	}
}

void CLuaCallbacks::OnFireGameEvent(CGameEvent* pEvent) {
	for (const auto& [name, callback] : Callbacks["FireGameEvent"])
	{
		if (callback.valid())
		{
			HandleError(callback(WGameEvent(pEvent)));
		}
	}
}

void CLuaCallbacks::OnUserMessage(int msgType, bf_read& msgData)
{
	for (const auto& [name, callback] : Callbacks["DispatchUserMessage"])
	{
		if (callback.valid())
		{
			HandleError(callback(WUserMessage(msgType, msgData)));
		}
	}
}

void CLuaCallbacks::OnFrameStage(int curStage)
{
	for (const auto& [name, callback] : Callbacks["FrameStageNotify"])
	{
		if (callback.valid())
		{
			HandleError(callback(curStage));
		}
	}
}

void CLuaCallbacks::OnAntiAim(CUserCmd* pCmd, bool* pSendPacket, bool isReal)
{
	for (const auto& [name, callback] : Callbacks["AntiAim"])
	{
		if (callback.valid())
		{
			HandleError(callback(WUserCmd(pCmd, pSendPacket), isReal));
		}
	}
}
