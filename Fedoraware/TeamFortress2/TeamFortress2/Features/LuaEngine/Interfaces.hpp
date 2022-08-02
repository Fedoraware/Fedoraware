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
	
}
