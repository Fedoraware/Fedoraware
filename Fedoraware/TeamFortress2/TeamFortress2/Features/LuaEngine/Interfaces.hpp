#pragma once
#include "../../SDK/SDK.h"
#include "Wrapper/Wrapper.hpp"

class ExportedInterfaces {
public:
	WEngineClient GetEngine()
	{
		static WEngineClient engineClient(I::EngineClient);
		return engineClient;
	}

	WBaseEntity GetLocalPlayer()
	{
		static WBaseEntity baseEntity(g_EntityCache.GetLocal());
		return baseEntity;
	}
};

namespace LuaUtils
{
	inline void Print(const char* msg)
	{
		I::Cvar->ConsolePrintf("%s\n", msg);
	}

	inline void Text(int x, int y, const char* text)
	{
		g_Draw.String(FONT_MENU, x, y, { 255, 255, 255, 255 }, EStringAlign::ALIGN_CENTER, "%s", text);
	}
}
