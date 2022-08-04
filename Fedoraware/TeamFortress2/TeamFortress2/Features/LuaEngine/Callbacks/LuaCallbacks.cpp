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

/* Unregisters an existing callback by name */
void CLuaCallbacks::Unregister(const char* type, const char* name)
{
	Callbacks[type].erase(name);
}

void CLuaCallbacks::Reset()
{
	Callbacks.clear();
}

/* Simple callback without any args */
void CLuaCallbacks::ByType(const char* type) {
	for (const auto& [name, callback] : Callbacks[type])
	{
		if (callback.valid())
		{
			callback();
		}
	}
}

void CLuaCallbacks::OnCreateMove(CUserCmd* pCmd, bool* pSendPacket) {
	for (const auto& [name, callback] : Callbacks["CreateMove"])
	{
		if (callback.valid())
		{
			callback(WUserCmd(pCmd, pSendPacket));
		}
	}
}

void CLuaCallbacks::OnFireGameEvent(CGameEvent* pEvent) {
	for (const auto& [name, callback] : Callbacks["CreateMove"])
	{
		if (callback.valid())
		{
			callback(WGameEvent(pEvent));
		}
	}
}
