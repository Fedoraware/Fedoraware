#include "LuaEngine.h"
#include "Callbacks/LuaCallbacks.h"
#include "Interfaces.hpp"
#include "../Commands/Commands.h"

#include <boost/algorithm/string/join.hpp>

sol::state LuaState;

/* Executes the given file | TODO: Use stl string */
void CLuaEngine::ExecuteFile(std::string file)
{
	LuaState.script_file(file);
}

/* Executes the given expression | TODO: Use stl string */
void CLuaEngine::ExecuteString(std::string expression)
{
	LuaState.script(expression);
}

void RegisterCallback(const char* type, const char* name, const sol::function& callback)
{
	F::LuaCallbacks.Register(type, name, callback);
}

void UnregisterCallback(const char* type, const char* name)
{
	F::LuaCallbacks.Unregister(type, name);
}

void CLuaEngine::Init()
{
	LuaState.open_libraries(sol::lib::base); // TODO: ?

	/* Initialize LuaBridge */
	{
		static ExportedDraw exDraw;
		static WEngineClient engineClient(I::EngineClient);

		// TODO: This

		// Vector3
		auto vecClass = LuaState.new_usertype<Vec3>("Vec3", sol::constructors<Vec3(), Vec3(float, float, float)>());
		vecClass["x"] = &Vec3::x;
		vecClass["y"] = &Vec3::y;
		vecClass["z"] = &Vec3::z;

		// CUserCmd
		auto userCmdClass = LuaState.new_usertype<WUserCmd>("UserCmd");
		userCmdClass["GetButtons"] = &WUserCmd::GetButtons;
		userCmdClass["GetViewAngles"] = &WUserCmd::GetViewAngles;
		userCmdClass["GetForwardMove"] = &WUserCmd::GetForwardMove;
		userCmdClass["GetSideMove"] = &WUserCmd::GetSideMove;
		userCmdClass["GetUpMove"] = &WUserCmd::GetUpMove;
		userCmdClass["SetButtons"] = &WUserCmd::SetButtons;
		userCmdClass["SetViewAngles"] = &WUserCmd::SetViewAngles;
		userCmdClass["SetForwardMove"] = &WUserCmd::SetForwardMove;
		userCmdClass["SetSideMove"] = &WUserCmd::SetSideMove;
		userCmdClass["SetUpMove"] = &WUserCmd::SetUpMove;

		// CEngineClient
		auto engineClass = LuaState.new_usertype<WEngineClient>("EngineClient");
		engineClass["IsInGame"] = &WEngineClient::IsInGame;
		engineClass["IsConnected"] = &WEngineClient::IsConnected;
		engineClass["IsTakingScreenshot"] = &WEngineClient::IsTakingScreenshot;
		engineClass["ExecuteCommand"] = &WEngineClient::ExecuteCommand;
		engineClass["GetLocalPlayer"] = &WEngineClient::GetLocalPlayer;
		engineClass["GetMaxClients"] = &WEngineClient::GetMaxClients;
		engineClass["GetLevelName"] = &WEngineClient::GetLevelName;
		engineClass["GetScreenSize"] = &WEngineClient::IsInGame;
		engineClass["GetViewAngles"] = &WEngineClient::GetViewAngles;
		engineClass["SetViewAngles"] = &WEngineClient::SetViewAngles;

		// CBaseEntity
		auto entityClass = LuaState.new_usertype<WBaseEntity>("BaseEntity");
		entityClass["IsValid"] = &WBaseEntity::IsValid;
		entityClass["GetIndex"] = &WBaseEntity::GetIndex;
		entityClass["GetOrigin"] = &WBaseEntity::GetOrigin;
		entityClass["GetClassID"] = &WBaseEntity::GetClassID;
		entityClass["GetClass"] = &WBaseEntity::GetClass;
		entityClass["GetHealth"] = &WBaseEntity::GetHealth;
		entityClass["GetAmmo"] = &WBaseEntity::GetAmmo;
		entityClass["GetFlags"] = &WBaseEntity::GetFlags;
		entityClass["GetEyePos"] = &WBaseEntity::GetEyePos;
		entityClass["IsDormant"] = &WBaseEntity::IsDormant;
		entityClass["IsAlive"] = &WBaseEntity::IsAlive;
		entityClass["GetTeam"] = &WBaseEntity::GetTeam;
		entityClass["SetOrigin"] = &WBaseEntity::SetOrigin;

		// CTFPlayerResource
		auto prClass = LuaState.new_usertype<WPlayerResource>("PlayerResource");
		prClass["GetPing"] = &WPlayerResource::GetPing;
		prClass["GetKills"] = &WPlayerResource::GetKills;
		prClass["GetDeaths"] = &WPlayerResource::GetDeaths;
		prClass["GetConnected"] = &WPlayerResource::GetConnected;
		prClass["GetValid"] = &WPlayerResource::GetValid;
		prClass["GetPlayerName"] = &WPlayerResource::GetPlayerName;
		prClass["GetDamage"] = &WPlayerResource::GetDamage;

		// Draw
		auto drawClass = LuaState.new_usertype<ExportedDraw>("DrawClass");
		drawClass["Text"] = &ExportedDraw::Text;
		drawClass["Line"] = &ExportedDraw::Line;
		drawClass["Rect"] = &ExportedDraw::Rect;
		drawClass["OutlinedRect"] = &ExportedDraw::OutlinedRect;
		drawClass["FilledCircle"] = &ExportedDraw::FilledCircle;
		drawClass["SetColor"] = &ExportedDraw::SetColor;

		// Entities
		auto entityTable = LuaState.create_named_table("Entities");
		entityTable["GetLocalPlayer"] = [] { return WBaseEntity(g_EntityCache.GetLocal()); };
		entityTable["GetLocalWeapon"] = [] { return WBaseEntity(g_EntityCache.GetWeapon()); };
		entityTable["GetPlayerResource"] = [] { return WPlayerResource(g_EntityCache.GetPR()); };
		entityTable["GetByIndex"] = [](int idx) { return WBaseEntity(I::ClientEntityList->GetClientEntity(idx)); };

		// Input
		auto inputTable = LuaState.create_named_table("Input");
		inputTable["IsKeyDown"] = [](int key) { return (GetAsyncKeyState(key) & 0x8000) != 0; };
		inputTable["GetMousePos"] = [] {
			int x, y;
			I::VGuiSurface->SurfaceGetCursorPos(x, y);
			return std::vector{ x, y };
		};

		// GlobalInfo
		auto globalInfo = LuaState.create_named_table("GlobalInfo");
		globalInfo["RealTime"] = [] { return I::GlobalVars->realtime; };
		globalInfo["FrameCount"] = [] { return I::GlobalVars->framecount; };
		globalInfo["AbsFrameTime"] = [] { return I::GlobalVars->absoluteframetime; };
		globalInfo["CurTime"] = [] { return I::GlobalVars->curtime; };
		globalInfo["FrameTime"] = [] { return I::GlobalVars->frametime; };
		globalInfo["MaxClients"] = [] { return I::GlobalVars->maxclients; };
		globalInfo["TickCount"] = [] { return I::GlobalVars->tickcount; };
		globalInfo["IntervalPerTick"] = [] { return I::GlobalVars->interval_per_tick; };

		// Fedoraware Globals
		auto fwareGlobals = LuaState.create_named_table("Fedoraware");
		fwareGlobals["ShiftedTicks"] = [] { return G::ShiftedTicks; };
		fwareGlobals["ShouldShift"] = [] { return G::ShouldShift; };
		fwareGlobals["CurrentTargetIdx"] = [] { return G::CurrentTargetIdx; };
		fwareGlobals["GetPriority"] = [](uint32_t friendsId) { return G::PlayerPriority[friendsId].Mode; };

		// Interfaces
		auto interfaceTable = LuaState.create_named_table("Interfaces");
		interfaceTable["Engine"] = &engineClient; // TODO: Make this readonly
		interfaceTable["Draw"] = &exDraw; // TODO: Make this readonly

		// Callbacks
		auto callbackTable = LuaState.create_named_table("Callbacks");
		callbackTable["Register"] = RegisterCallback;
		callbackTable["Unregister"] = UnregisterCallback;

		// Global functions
		LuaState.set_function("print", [](const char* msg) { I::Cvar->ConsolePrintf("%s\n", msg); });
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

void CLuaEngine::Reset()
{
	F::LuaCallbacks.Reset();
	LuaState = { };
}
