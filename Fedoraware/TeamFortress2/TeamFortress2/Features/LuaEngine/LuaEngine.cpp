#include "LuaEngine.h"
#include "LuaCallbacks.hpp"
#include "Interfaces.hpp"
#include "../Commands/Commands.h"

#include <boost/algorithm/string/join.hpp>

/* Prints the last lua error */
void CLuaEngine::PrintError()
{
	std::string errorMessage = "LUA Error: ";
	errorMessage.append(lua_tostring(LuaState, -1));
	errorMessage.append("\n");

	I::Cvar->ConsoleColorPrintf({235, 59, 90, 255}, "%s", errorMessage.c_str());
	lua_pop(LuaState, -1);
}

/* Executes the given file */
void CLuaEngine::ExecuteFile(const char* file)
{
	if (!file || !LuaState) { return; }

	if (luaL_dofile(LuaState, file))
	{
		PrintError();
	}
}

/* Executes the given expression */
void CLuaEngine::ExecuteString(const char* expression)
{
	if (!expression || !LuaState)
	{
		I::Cvar->ConsoleColorPrintf({235, 59, 90, 255}, "Lua Error: null expression passed to script engine\n");
		return;
	}

	if (luaL_dostring(LuaState, expression))
	{
		PrintError();
	}
}

void RegisterCallback(const char* type, const char* name, const luabridge::LuaRef& callback)
{
	F::LuaCallbacks.Register(type, name, callback);
}

void UnregisterCallback(const char* type, const char* name)
{
	F::LuaCallbacks.Unregister(type, name);
}

void CLuaEngine::Init()
{
	LOCKLUA();

	/* Initialize LuaBridge */
	{
		static ExportedDraw exDraw;
		static WEngineClient engineClient(I::EngineClient);

		using namespace luabridge;
		getGlobalNamespace(LuaState)
			/* Utils */
			.beginNamespace("Utils")

			// Vec3
			.beginClass<Vec3>("Vec3")
			.addConstructor<void(*)(float, float, float)>()
			.addProperty("x", &Vec3::x)
			.addProperty("y", &Vec3::y)
			.addProperty("z", &Vec3::z)
			.endClass()

			.endNamespace() // Utils

			/* Interfaces */
			.beginNamespace("Interfaces")

			// CUserCmd
			.beginClass<WUserCmd>("UserCmd")
			.addFunction("GetButtons", &WUserCmd::GetButtons)
			.addFunction("GetViewAngles", &WUserCmd::GetViewAngles)
			.addFunction("GetForwardMove", &WUserCmd::GetForwardMove)
			.addFunction("GetSideMove", &WUserCmd::GetSideMove)
			.addFunction("GetUpMove", &WUserCmd::GetUpMove)

			.addFunction("SetButtons", &WUserCmd::SetButtons)
			.addFunction("SetViewAngles", &WUserCmd::SetViewAngles)
			.addFunction("SetForwardMove", &WUserCmd::SetForwardMove)
			.addFunction("SetSideMove", &WUserCmd::SetSideMove)
			.addFunction("SetUpMove", &WUserCmd::SetUpMove)
			.endClass()

			// CEngineClient
			.beginClass<WEngineClient>("EngineClient")
			.addFunction("IsInGame", &WEngineClient::IsInGame)
			.addFunction("IsConnected", &WEngineClient::IsConnected)
			.addFunction("IsTakingScreenshot", &WEngineClient::IsTakingScreenshot)
			.addFunction("ExecuteCommand", &WEngineClient::ExecuteCommand)
			.addFunction("GetLocalPlayer", &WEngineClient::GetLocalPlayer)
			.addFunction("GetMaxClients", &WEngineClient::GetMaxClients)
			.addFunction("GetLevelName", &WEngineClient::GetLevelName)
			.addFunction("GetScreenSize", &WEngineClient::GetScreenSize)
			.addFunction("GetViewAngles", &WEngineClient::GetViewAngles)
			.addFunction("SetViewAngles", &WEngineClient::SetViewAngles)
			.endClass()

			// CBaseEntity
			.beginClass<WBaseEntity>("BaseEntity")
			.addFunction("IsValid", &WBaseEntity::IsValid)
			.addFunction("GetIndex", &WBaseEntity::GetIndex)
			.addFunction("GetOrigin", &WBaseEntity::GetOrigin)
			.addFunction("GetClassID", &WBaseEntity::GetClassID)
			.addFunction("GetClassName", &WBaseEntity::GetClass)
			.addFunction("GetHealth", &WBaseEntity::GetHealth)
			.addFunction("GetAmmo", &WBaseEntity::GetAmmo)
			.addFunction("GetFlags", &WBaseEntity::GetFlags)
			.addFunction("GetEyePos", &WBaseEntity::GetEyePos)
			.addFunction("IsDormant", &WBaseEntity::IsDormant)
			.addFunction("IsAlive", &WBaseEntity::IsAlive)
			.addFunction("GetTeam", &WBaseEntity::GetTeam)
			.addFunction("SetOrigin", &WBaseEntity::SetOrigin)
			.endClass()

			// CTFPlayerResource
			.beginClass<WPlayerResource>("PlayerResource")
			.addFunction("GetPing", &WPlayerResource::GetPing)
			.addFunction("GetKills", &WPlayerResource::GetKills)
			.addFunction("GetDeaths", &WPlayerResource::GetDeaths)
			.addFunction("GetConnected", &WPlayerResource::GetConnected)
			.addFunction("GetValid", &WPlayerResource::GetValid)
			.addFunction("GetPlayerName", &WPlayerResource::GetPlayerName)
			.addFunction("GetDamage", &WPlayerResource::GetDamage)
			.endClass()

			// Draw
			.beginClass<ExportedDraw>("DrawClass")
			.addFunction("Text", &ExportedDraw::Text)
			.addFunction("Line", &ExportedDraw::Line)
			.addFunction("Rect", &ExportedDraw::Rect)
			.addFunction("OutlinedRect", &ExportedDraw::OutlinedRect)
			.addFunction("FilledCircle", &ExportedDraw::FilledCircle)
			.addFunction("SetColor", &ExportedDraw::SetColor)
			.endClass()

			// Global Vars, Props and Functions
			.addProperty("Engine", &engineClient)
			.addProperty("Draw", &exDraw, false)
			.endNamespace() // Interfaces

			// Entities
			.beginNamespace("Entities")
			.addFunction("GetLocalPlayer", +[] { return WBaseEntity(g_EntityCache.GetLocal()); })
			.addFunction("GetLocalWeapon", +[] { return WBaseEntity(g_EntityCache.GetWeapon()); })
			.addFunction("GetPlayerResource", +[] { return WPlayerResource(g_EntityCache.GetPR()); })
			.addFunction("GetByIndex", +[](int idx) { return WBaseEntity(I::ClientEntityList->GetClientEntity(idx)); })
			.endNamespace() // Entities

			// Input
			.beginNamespace("Input")
			.addFunction("IsKeyDown", +[](int key) { return (GetAsyncKeyState(key) & 0x8000) != 0; })
			.addFunction("GetMousePos", +[] { int p[2]{ }; I::VGuiSurface->SurfaceGetCursorPos(p[0], p[1]); return p; })
			.endNamespace() // Input

			// GlobalInfo
			.beginNamespace("GlobalInfo")
			.addFunction("RealTime", +[] { return I::GlobalVars->realtime; })
			.addFunction("FrameCount", +[] { return I::GlobalVars->framecount; })
			.addFunction("AbsFrameTime", +[] { return I::GlobalVars->absoluteframetime; })
			.addFunction("CurTime", +[] { return I::GlobalVars->curtime; })
			.addFunction("FrameTime", +[] { return I::GlobalVars->frametime; })
			.addFunction("MaxClients", +[] { return I::GlobalVars->maxclients; })
			.addFunction("TickCount", +[] { return I::GlobalVars->tickcount; })
			.addFunction("IntervalPerTick", +[] { return I::GlobalVars->interval_per_tick; })
			.endNamespace() // GlobalInfo

			// Fedoraware Globals
			.beginNamespace("Fedoraware")
			.addProperty("ShiftedTicks", &G::ShiftedTicks, false)
			.addProperty("ShouldShift", &G::ShouldShift, false)
			.addProperty("CurrentTargetIdx", &G::CurrentTargetIdx, false)
			.addFunction("GetPriority", +[](uint32_t friendsId) { return G::PlayerPriority[friendsId].Mode; })
			.endNamespace() // Fedoraware

			// Callbacks
			.beginNamespace("Callbacks")
			.addFunction("Register", RegisterCallback)
			.addFunction("Unregister", UnregisterCallback)
			.endNamespace() // Callbacks

			/* Global functions */
			.addFunction("print", +[](const char* msg) { I::Cvar->ConsolePrintf("%s\n", msg); });
	}

	/* Register commands */
	{
		F::Commands.Register("lua_load", [&](const std::deque<std::string>& args) {
			if (args.empty())
			{
				F::Commands.Error("Usage: lua_load <path>");
				return;
			}

			ExecuteFile(args.front().c_str());
		});

		F::Commands.Register("lua_do", [&](const std::deque<std::string>& args) {
			if (args.empty())
			{
				F::Commands.Error("Usage: lua_do <expression>");
				return;
			}

			const std::string expr = boost::algorithm::join(args, " ");
			ExecuteString(expr.c_str());
		});
	}
}
