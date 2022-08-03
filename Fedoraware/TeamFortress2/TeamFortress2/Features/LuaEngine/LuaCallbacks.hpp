#pragma once
#include <lua.hpp>
#include <LuaBridge.h>

#include "../../SDK/SDK.h"
#include "Wrapper/Wrapper.hpp"

class CLuaCallbacks {
	std::unordered_map<std::string, std::unordered_map<std::string, luabridge::LuaRef>> Callbacks;

	void ProcessResult(const luabridge::LuaResult& cbResult)
	{
		if (!cbResult.hasFailed()) { return; }

		I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, "%s", cbResult.errorMessage().c_str());
	}

public:
	/* Registers a new Lua callback of the given type */
	void Register(const char* type, const char* name, const luabridge::LuaRef& callback)
	{
		if (callback.isFunction())
		{
			Callbacks[type].emplace(name, callback);
		}
	}

	/* Unregisters an existing callback by name */
	void Unregister(const char* type, const char* name)
	{
		Callbacks[type].erase(name);
	}

	/* Simple callback without any args */
	void ByType(const char* type)
	{
		for (const auto& [name, callback] : Callbacks[type])
		{
			if (callback.isValid())
			{
				ProcessResult(callback());
			}
		}
	}

	void OnCreateMove(CUserCmd* pCmd, bool* pSendPacket)
	{
		for (const auto& [name, callback] : Callbacks["CreateMove"])
		{
			if (callback.isValid())
			{
				ProcessResult(callback(WUserCmd(pCmd, pSendPacket)));
			}
		}
	}

	void OnFireGameEvent(CGameEvent* pEvent)
	{
		for (const auto& [name, callback] : Callbacks["CreateMove"])
		{
			if (callback.isValid())
			{
				ProcessResult(callback(WGameEvent(pEvent)));
			}
		}
	}
};

ADD_FEATURE(CLuaCallbacks, LuaCallbacks)