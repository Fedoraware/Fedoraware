#include "LuaEngine.h"

#include <lua.hpp>
#include <sol/sol.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Interfaces.hpp"
#include "Callbacks/LuaCallbacks.h"
#include "../Commands/Commands.h"

static sol::state LuaState;

/* Executes the given file */
void CLuaEngine::ExecuteFile(const std::string& file)
{
	const auto result = LuaState.safe_script_file(file, &sol::script_pass_on_error);
	HandleError(result);
}

/* Executes the given expression */
void CLuaEngine::ExecuteString(const std::string& expression)
{
	LuaState.safe_script(expression);
}

void CLuaEngine::HandleError(const sol::protected_function_result& result)
{
	if (!result.valid())
	{
		const sol::error err = result;
		I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, "%s\n", err.what());
	}
}

void RegisterCallback(const char* type, const char* name, const sol::function& callback)
{
	F::LuaCallbacks.Register(type, name, callback);
}

void UnregisterCallback(const char* type, const char* name)
{
	F::LuaCallbacks.Unregister(type, name);
}

void OnPanic(sol::optional<std::string> errorMsg)
{
	if (errorMsg) {
		const auto& msg = errorMsg.value();
		I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, "Panic:\n%s", msg.c_str());
	}
}

void CLuaEngine::Init()
{
	LuaState.open_libraries(sol::lib::base, sol::lib::string, sol::lib::ffi, sol::lib::io, sol::lib::math);
	LuaState.set_panic(sol::c_call<decltype(&OnPanic), &OnPanic>);

	/* Initialize LuaBridge */
	{
		static ExportedDraw exDraw;
		static WEngineClient engineClient(I::EngineClient);

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
		engineClass["GetScreenSize"] = &WEngineClient::GetScreenSize;
		engineClass["GetViewAngles"] = &WEngineClient::GetViewAngles;
		engineClass["SetViewAngles"] = &WEngineClient::SetViewAngles;

		// CBaseEntity
		auto entityClass = LuaState.new_usertype<WBaseEntity>("BaseEntity", sol::constructors<WBaseEntity(CBaseEntity*)>());
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

		// CBaseCombatWeapon
		auto weaponClass = LuaState.new_usertype<WBaseCombatWeapon>("BaseCombatWeapon");
		weaponClass["IsValid"] = &WBaseCombatWeapon::IsValid;
		weaponClass["GetEntity"] = &WBaseCombatWeapon::GetEntity;
		weaponClass["GetName"] = &WBaseCombatWeapon::GetName;
		weaponClass["CanShoot"] = &WBaseCombatWeapon::CanShoot;
		weaponClass["GetClip1"] = &WBaseCombatWeapon::GetClip1;
		weaponClass["GetClip2"] = &WBaseCombatWeapon::GetClip2;
		weaponClass["GetSlot"] = &WBaseCombatWeapon::GetSlot;
		weaponClass["GetWeaponID"] = &WBaseCombatWeapon::GetWeaponID;
		weaponClass["IsInReload"] = &WBaseCombatWeapon::IsInReload;

		// CGameEvent
		auto eventClass = LuaState.new_usertype<WGameEvent>("GameEvent");
		eventClass["IsValid"] = &WGameEvent::IsValid;
		eventClass["GetName"] = &WGameEvent::GetName;
		eventClass["GetBool"] = &WGameEvent::GetBool;
		eventClass["GetInt"] = &WGameEvent::GetInt;
		eventClass["GetFloat"] = &WGameEvent::GetFloat;
		eventClass["GetString"] = &WGameEvent::GetString;
		eventClass["SetBool"] = &WGameEvent::SetBool;
		eventClass["SetInt"] = &WGameEvent::SetInt;
		eventClass["SetFloat"] = &WGameEvent::SetFloat;
		eventClass["SetString"] = &WGameEvent::SetString;

		// CTFPlayerResource
		auto prClass = LuaState.new_usertype<WPlayerResource>("PlayerResource");
		prClass["GetPing"] = &WPlayerResource::GetPing;
		prClass["GetKills"] = &WPlayerResource::GetKills;
		prClass["GetDeaths"] = &WPlayerResource::GetDeaths;
		prClass["GetConnected"] = &WPlayerResource::GetConnected;
		prClass["GetValid"] = &WPlayerResource::GetValid;
		prClass["GetPlayerName"] = &WPlayerResource::GetPlayerName;
		prClass["GetDamage"] = &WPlayerResource::GetDamage;

		// UserMessage
		auto userMsgClass = LuaState.new_usertype<WUserMessage>("UserMessage");
		userMsgClass["GetType"] = &WUserMessage::GetType;
		userMsgClass["Reset"] = &WUserMessage::Reset;
		userMsgClass["Seek"] = &WUserMessage::Seek;
		userMsgClass["GetDataBits"] = &WUserMessage::GetDataBits;
		userMsgClass["GetNumBitsLeft"] = &WUserMessage::GetNumBitsLeft;
		userMsgClass["ReadByte"] = &WUserMessage::ReadByte;
		userMsgClass["ReadFloat"] = &WUserMessage::ReadFloat;
		userMsgClass["ReadString"] = &WUserMessage::ReadString;

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
		entityTable["GetLocalWeapon"] = [] { return WBaseCombatWeapon(g_EntityCache.GetWeapon()); };
		entityTable["GetPlayerResource"] = [] { return WPlayerResource(g_EntityCache.GetPR()); };
		entityTable["GetByIndex"] = [](int idx) { return WBaseEntity(I::ClientEntityList->GetClientEntity(idx)); };

		// Input
		auto inputTable = LuaState.create_named_table("Input");
		inputTable["IsDown"] = [](int key) { return (GetAsyncKeyState(key) & 0x8000) != 0; };
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

		// Enums
		auto enums = LuaState.create_named_table("Enums");
		enums["Buttons"] = LuaState.create_table_with(
			"IN_ATTACK", 1 << 0,
			"IN_JUMP", 1 << 1,
			"IN_DUCK", 1 << 2,
			"IN_FORWARD", 1 << 3,
			"IN_BACK", 1 << 4,
			"IN_USE", 1 << 5,
			"IN_CANCEL", 1 << 6,
			"IN_LEFT", 1 << 7,
			"IN_RIGHT", 1 << 8,
			"IN_MOVELEFT", 1 << 9,
			"IN_MOVERIGHT", 1 << 10,
			"IN_ATTACK2", 1 << 11,
			"IN_RUN", 1 << 12,
			"IN_RELOAD", 1 << 13,
			"IN_ALT1", 1 << 14,
			"IN_ALT2", 1 << 15,
			"IN_SCORE", 1 << 16,
			"IN_SPEED", 1 << 17,
			"IN_WALK", 1 << 18,
			"IN_ZOOM", 1 << 19,
			"IN_WEAPON1", 1 << 20,
			"IN_WEAPON2", 1 << 21,
			"IN_BULLRUSH", 1 << 22,
			"IN_GRENADE1", 1 << 23,
			"IN_GRENADE2", 1 << 24,
			"IN_ATTACK3", 1 << 25
		);

		// Interfaces
		auto interfaceTable = LuaState.create_named_table("Interfaces");
		interfaceTable["GetEngine"] = [] { return &engineClient; };
		interfaceTable["GetDraw"] = [] { return exDraw; };

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

			ExecuteFile(args.front());
		});

		F::Commands.Register("lua_do", [&](const std::deque<std::string>& args) {
			if (args.empty())
			{
				F::Commands.Error("Usage: lua_do <expression>");
				return;
			}

			const std::string expr = boost::algorithm::join(args, " ");
			ExecuteString(expr);
		});
	}
}

void CLuaEngine::Reset()
{
	F::LuaCallbacks.Reset();
	LuaState = { };
}
