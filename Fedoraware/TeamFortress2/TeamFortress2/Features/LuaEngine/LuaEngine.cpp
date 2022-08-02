#include "LuaEngine.h"
#include <boost/algorithm/string/join.hpp>
#include "../Commands/Commands.h"

std::unordered_map<std::string, std::unordered_map<std::string, luabridge::LuaRef>> Callbacks;

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
		I::Cvar->ConsoleColorPrintf({235, 59, 90, 255}, _("Lua Error: null expression passed to script engine\n"));
		return;
	}

	if (luaL_dostring(LuaState, expression))
	{
		PrintError();
	}
}

std::unordered_map<std::string, luabridge::LuaRef>& CLuaEngine::GetCallbacks(const std::string& type)
{
	return Callbacks[type];
}

void RegisterCallback(const char* type, const char* name, const luabridge::LuaRef& callback)
{
	if (callback.isFunction())
	{
		Callbacks[type].emplace(name, callback);
	}
}

void UnregisterCallback(const char* type, const char* name)
{
	Callbacks[type].erase(name);
}

void Print(const char* msg)
{
	I::Cvar->ConsolePrintf("%s\n", msg);
}

void CLuaEngine::Init()
{
	LOCKLUA();

	/* Initialize LuaBridge */
	{
		static ExportedInterfaces exInterfaces;
		static ExportedDraw exDraw;

		using namespace luabridge;
		getGlobalNamespace(LuaState)
			.beginNamespace("Game")

			/* Utils */
			.beginNamespace("Utils")

			// Vec3
			.beginClass<Vec3>("Vec3")
			.addConstructor<void(*)(float, float, float)>()
			.addProperty("x", &Vec3::x)
			.addProperty("y", &Vec3::y)
			.addProperty("z", &Vec3::z)
			.endClass()

			.endNamespace() // Utils Namespace

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

			// Draw
			.beginClass<ExportedDraw>("DrawClass")
			.addFunction("Text", &ExportedDraw::Text)
			.addFunction("Line", &ExportedDraw::Line)
			.addFunction("Rect", &ExportedDraw::Rect)
			.addFunction("OutlinedRect", &ExportedDraw::OutlinedRect)
			.addFunction("FilledCircle", &ExportedDraw::FilledCircle)
			.addFunction("SetColor", &ExportedDraw::SetColor)
			.endClass()

			// Interfaces
			.beginClass<ExportedInterfaces>("InterfaceClass")
			.addFunction("GetEngine", &ExportedInterfaces::GetEngine)
			.addFunction("GetLocalPlayer", &ExportedInterfaces::GetLocalPlayer)
			.endClass()

			// Global Vars, Props and Functions
			.addProperty("Interfaces", &exInterfaces, false)
			.addProperty("Draw", &exDraw, false)
			.endNamespace() // Game Namespace

			.beginNamespace("GlobalInfo")
			.addFunction("RealTime", +[] { return I::GlobalVars->realtime; })
			.addFunction("FrameCount", +[] { return I::GlobalVars->framecount; })
			.addFunction("AbsFrameTime", +[] { return I::GlobalVars->absoluteframetime; })
			.addFunction("CurTime", +[] { return I::GlobalVars->curtime; })
			.addFunction("FrameTime", +[] { return I::GlobalVars->frametime; })
			.addFunction("MaxClients", +[] { return I::GlobalVars->maxclients; })
			.addFunction("TickCount", +[] { return I::GlobalVars->tickcount; })
			.addFunction("IntervalPerTick", +[] { return I::GlobalVars->interval_per_tick; })
			.endNamespace()

			.beginNamespace("Fedoraware")
			.addProperty("ShiftedTicks", &G::ShiftedTicks, false)
			.addProperty("ShouldShift", &G::ShouldShift, false)
			.endNamespace()

			// Callbacks
			.beginNamespace("Callbacks")
			.addFunction("Register", RegisterCallback)
			.addFunction("Unregister", UnregisterCallback)
			.endNamespace()

			/* Global functions */
			.addFunction("print", Print);
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
