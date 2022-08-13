#include "LuaEngine.h"

#include <lua.hpp>
#include <sol/sol.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Wrapper.hpp"
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
	const auto result = LuaState.safe_script(expression, &sol::script_pass_on_error);
	HandleError(result);
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
		static WDraw draw;
		static WEngineClient engineClient(I::EngineClient);
		static WClientMode clientMode(I::ClientModeShared);

		// Vector3
		auto vecClass = LuaState.new_usertype<Vec3>("Vec3", sol::constructors<Vec3(), Vec3(float, float, float)>());
		vecClass["x"] = &Vec3::x;
		vecClass["y"] = &Vec3::y;
		vecClass["z"] = &Vec3::z;
		vecClass["Length"] = &Vec3::Length;
		vecClass["LengthSqr"] = &Vec3::LengthSqr;
		vecClass["Normalize"] = &Vec3::Normalize;
		vecClass["Length2D"] = &Vec3::Length2D;
		vecClass["DistTo"] = &Vec3::DistTo;
		vecClass["DistToSqr"] = &Vec3::DistToSqr;
		vecClass["Dot"] = &Vec3::Dot;
		vecClass["Cross"] = &Vec3::Cross;
		vecClass["IsZero"] = &Vec3::IsZero;
		
		// Vec2
		auto vec2Class = LuaState.new_usertype<Vec2>("Vec2", sol::constructors<Vec2(), Vec2(float, float)>());
		vec2Class["x"] = &Vec2::x;
		vec2Class["y"] = &Vec2::y;
		vec2Class["Normalize"] = &Vec2::Normalize;
		vec2Class["Length"] = &Vec2::Length;
		vec2Class["LengthSqr"] = &Vec2::LengthSqr;
		vec2Class["DistTo"] = &Vec2::DistTo;
		vec2Class["DistToSqr"] = &Vec2::DistToSqr;
		vec2Class["Dot"] = &Vec2::Dot;

		// CUserCmd
		auto userCmdClass = LuaState.new_usertype<WUserCmd>("UserCmd");
		userCmdClass["IsValid"] = &WUserCmd::IsValid;
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
		userCmdClass["SetSendPacket"] = &WUserCmd::SetSendPacket;

		// CEngineClient
		auto engineClass = LuaState.new_usertype<WEngineClient>("EngineClient");
		engineClass["IsValid"] = &WEngineClient::IsValid;
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
		engineClass["GetPlayerForUserID"] = &WEngineClient::GetPlayerForUserID;

		// CClientModeShared
		auto clientClass = LuaState.new_usertype<WClientMode>("ClientMode");
		clientClass["ChatPrintf"] = &WClientMode::ChatPrintf;

		// CBaseEntity
		auto entityClass = LuaState.new_usertype<WBaseEntity>("BaseEntity", sol::constructors<WBaseEntity(CBaseEntity*)>());
		entityClass["IsValid"] = &WBaseEntity::IsValid;
		entityClass["GetIndex"] = &WBaseEntity::GetIndex;
		entityClass["GetOrigin"] = &WBaseEntity::GetOrigin;
		entityClass["GetAngles"] = &WBaseEntity::GetAngles;
		entityClass["GetEyeAngles"] = &WBaseEntity::GetEyeAngles;
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
		entityClass["SetAngles"] = &WBaseEntity::SetAngles;
		entityClass["SetEyeAngles"] = &WBaseEntity::SetEyeAngles;
		entityClass["GetCriticals"] = &WBaseEntity::GetCriticals;

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
		weaponClass["GetDamage"] = &WBaseCombatWeapon::GetDamage;
		weaponClass["GetBulletsPerShot"] = &WBaseCombatWeapon::GetBulletsPerShot;

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
		prClass["GetTeam"] = &WPlayerResource::GetTeam;
		prClass["IsAlive"] = &WPlayerResource::IsAlive;
		prClass["GetHealth"] = &WPlayerResource::GetHealth;
		prClass["GetAccountID"] = &WPlayerResource::GetAccountID;
		prClass["GetValid"] = &WPlayerResource::GetValid;
		prClass["GetPlayerName"] = &WPlayerResource::GetPlayerName;
		prClass["GetScore"] = &WPlayerResource::GetScore;
		prClass["GetDamage"] = &WPlayerResource::GetDamage;
		prClass["GetMaxHealth"] = &WPlayerResource::GetMaxHealth;

		// UserMessage
		auto userMsgClass = LuaState.new_usertype<WUserMessage>("UserMessage");
		userMsgClass["GetType"] = &WUserMessage::GetType;
		userMsgClass["Reset"] = &WUserMessage::Reset;
		userMsgClass["Seek"] = &WUserMessage::Seek;
		userMsgClass["GetDataBits"] = &WUserMessage::GetDataBits;
		userMsgClass["GetNumBitsLeft"] = &WUserMessage::GetNumBitsLeft;
		userMsgClass["ReadByte"] = &WUserMessage::ReadByte;
		userMsgClass["ReadFloat"] = &WUserMessage::ReadFloat;
		userMsgClass["ReadLong"] = &WUserMessage::ReadLong;
		userMsgClass["ReadString"] = &WUserMessage::ReadString;

		// Draw
		auto drawClass = LuaState.new_usertype<WDraw>("DrawClass");
		drawClass["Text"] = &WDraw::Text;
		drawClass["Line"] = &WDraw::Line;
		drawClass["Rect"] = &WDraw::Rect;
		drawClass["OutlinedRect"] = &WDraw::OutlinedRect;
		drawClass["FilledCircle"] = &WDraw::FilledCircle;
		drawClass["SetColor"] = &WDraw::SetColor;
		drawClass["SetFont"] = &WDraw::SetFont;

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
			return Vec2(x, y);
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
		fwareGlobals["SilentTime"] = [] { return G::SilentTime; };
		fwareGlobals["SetSilentTime"] = [](bool state) { G::SilentTime = state; };

		// Enums
		auto enums = LuaState.create_named_table("Enums");
		enums["Buttons"] = LuaState.create_table_with(
			"IN_ATTACK", IN_ATTACK,
			"IN_JUMP", IN_JUMP,
			"IN_DUCK", IN_DUCK,
			"IN_FORWARD", IN_FORWARD,
			"IN_BACK", IN_BACK,
			"IN_USE", IN_USE,
			"IN_CANCEL", IN_CANCEL,
			"IN_LEFT", IN_LEFT,
			"IN_RIGHT", IN_RIGHT,
			"IN_MOVELEFT", IN_MOVELEFT,
			"IN_MOVERIGHT", IN_MOVERIGHT,
			"IN_ATTACK2", IN_ATTACK2,
			"IN_RUN", IN_RUN,
			"IN_RELOAD", IN_RELOAD,
			"IN_ALT1", IN_AL1,
			"IN_ALT2", IN_ALT2,
			"IN_SCORE", IN_SCORE,
			"IN_SPEED", IN_SPEED,
			"IN_WALK", IN_WALK,
			"IN_ZOOM", IN_ZOOM,
			"IN_WEAPON1", IN_WEAPON1,
			"IN_WEAPON2", IN_WEAPON2,
			"IN_BULLRUSH", IN_BULLRUSH,
			"IN_GRENADE1", IN_GRENADE1,
			"IN_GRENADE2", IN_GRENADE2,
			"IN_ATTACK3", IN_ATTACK3
		);
		enums["ClientFrameStage"] = LuaState.create_table_with(
			"FRAME_UNDEFINED", EClientFrameStage::FRAME_UNDEFINED,
			"FRAME_START", EClientFrameStage::FRAME_START,
			"FRAME_NET_UPDATE_START", EClientFrameStage::FRAME_NET_UPDATE_START,
			"FRAME_NET_UPDATE_POSTDATAUPDATE_START", EClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_START,
			"FRAME_NET_UPDATE_POSTDATAUPDATE_END", EClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_END,
			"FRAME_NET_UPDATE_END", EClientFrameStage::FRAME_NET_UPDATE_END,
			"FRAME_RENDER_START", EClientFrameStage::FRAME_RENDER_START,
			"FRAME_RENDER_END", EClientFrameStage::FRAME_RENDER_END
		);
		enums["Fonts"] = LuaState.create_table_with(
			"FONT_ESP", FONT_ESP,
			"FONT_ESP_NAME", FONT_ESP_NAME,
			"FONT_ESP_COND", FONT_ESP_COND,
			"FONT_ESP_PICKUPS", FONT_ESP_PICKUPS,
			"FONT_MENU", FONT_MENU,
			"FONT_INDICATORS", FONT_INDICATORS,
			"FONT_IMGUI", FONT_IMGUI,
			"FONT_OSRS", FONT_OSRS
		);

		// Interfaces
		auto interfaceTable = LuaState.create_named_table("Interfaces");
		interfaceTable["GetEngine"] = [] { return &engineClient; };
		interfaceTable["GetClient"] = [] { return &clientMode; };
		interfaceTable["GetDraw"] = [] { return &draw; };

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
