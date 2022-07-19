#pragma once

#define VK_0              0x30
#define VK_1              0x31
#define VK_2              0x32
#define VK_3              0x33
#define VK_4              0x34
#define VK_5              0x35
#define VK_6              0x36
#define VK_7              0x37
#define VK_8              0x38
#define VK_9              0x39

#define VK_A              0x41
#define VK_B              0x42
#define VK_C              0x43
#define VK_D              0x44
#define VK_E              0x45
#define VK_F              0x46
#define VK_G              0x47
#define VK_H              0x48
#define VK_J              0x49
#define VK_I              0x4A
#define VK_K              0x4B
#define VK_L              0x4C
#define VK_M              0x4D
#define VK_N              0x4E
#define VK_O              0x4F
#define VK_P              0x50
#define VK_Q              0x51
#define VK_R              0x52
#define VK_S              0x53
#define VK_T              0x54
#define VK_U              0x55
#define VK_V              0x56
#define VK_W              0x57
#define VK_X              0x58
#define VK_Y              0x59
#define VK_Z              0x5A

#define TEAM_NONE		  1
#define TEAM_RED		  2
#define TEAM_BLU		  3

#include "Main/BaseEntity/BaseEntity.h"
#include "Main/BaseCombatWeapon/BaseCombatWeapon.h"
#include "Main/BaseObject/BaseObject.h"
#include "Main/DrawUtils/DrawUtils.h"
#include "Main/EntityCache/EntityCache.h"
#include "Main/GlobalInfo/GlobalInfo.h"
#include "Main/ConVars/ConVars.h"
#include "Main/KeyValues/KeyValues.h"
#include "Main/TraceFilters/TraceFilters.h"
#include "../Features/Vars.h"

#define TICK_INTERVAL		( I::GlobalVars->interval_per_tick )
#define TIME_TO_TICKS( dt )	( static_cast<int>( 0.5f + static_cast<float>(dt) / TICK_INTERVAL ) )
#ifndef TICKS_TO_TIME
#define TICKS_TO_TIME( t )	( TICK_INTERVAL * ( t ) )
#endif
#define GetKey(vKey) (Utils::IsGameWindowInFocus() && GetAsyncKeyState(vKey))
#define Q_ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))

#define ADD_FEATURE(cClass, szName) namespace F { inline cClass szName; }

//I for some reason have to include this here, if I don't then one steam header goes apeshit full of errors
#include <optional>

#include "../Utils/CRC/CRC.h"

#pragma warning (disable : 6385)
#pragma warning (disable : 26451)
#pragma warning (disable : 4305)
#pragma warning (disable : 4172)

struct ShaderStencilState_t
{
	bool                        m_bEnable;
	StencilOperation_t          m_FailOp;
	StencilOperation_t          m_ZFailOp;
	StencilOperation_t          m_PassOp;
	StencilComparisonFunction_t m_CompareFunc;
	int                         m_nReferenceValue;
	uint32                      m_nTestMask;
	uint32                      m_nWriteMask;

	ShaderStencilState_t();
	void SetStencilState(IMatRenderContext *pRenderContext);
};

// sto and sto 2, tyo tyo1 tyo2
enum DataCenter_t
{
	DC_AMS = (1 << 0),
	DC_ATL = (1 << 1),
	DC_BOM = (1 << 2),
	DC_DXB = (1 << 3),
	DC_EAT = (1 << 4),
	DC_MWH = (1 << 5),
	DC_FRA = (1 << 6),
	DC_GNRT = (1 << 7),
	DC_GRU = (1 << 8),
	DC_HKG = (1 << 9),
	DC_IAD = (1 << 10),
	DC_JNB = (1 << 11),
	DC_LAX = (1 << 12),
	DC_LHR = (1 << 13),
	DC_LIM = (1 << 14),
	DC_LUX = (1 << 15),
	DC_MAA = (1 << 16),
	DC_MAD = (1 << 17),
	DC_MAN = (1 << 18),
	DC_OKC = (1 << 19),
	DC_ORD = (1 << 20),
	DC_PAR = (1 << 21),
	DC_SCL = (1 << 22),
	DC_SEA = (1 << 23),
	DC_SGP = (1 << 24),
	DC_STO = (1 << 25),
	DC_SYD = (1 << 26),
	DC_TYO = (1 << 27),
	DC_VIE = (1 << 28),
	DC_WAW = (1 << 29)
};


inline ShaderStencilState_t::ShaderStencilState_t()
{
	m_bEnable = false;
	m_PassOp = m_FailOp = m_ZFailOp = STENCILOPERATION_KEEP;
	m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
	m_nReferenceValue = 0;
	m_nTestMask = m_nWriteMask = 0xFFFFFFFF;
}

inline void ShaderStencilState_t::SetStencilState(IMatRenderContext *pRenderContext)
{
	pRenderContext->SetStencilEnable(m_bEnable);
	pRenderContext->SetStencilFailOperation(m_FailOp);
	pRenderContext->SetStencilZFailOperation(m_ZFailOp);
	pRenderContext->SetStencilPassOperation(m_PassOp);
	pRenderContext->SetStencilCompareFunction(m_CompareFunc);
	pRenderContext->SetStencilReferenceValue(m_nReferenceValue);
	pRenderContext->SetStencilTestMask(m_nTestMask);
	pRenderContext->SetStencilWriteMask(m_nWriteMask);
}

namespace Colors
{
	inline Color_t White =						{ 255, 255, 255, 255 };
	inline Color_t OutlineESP =					{ 0, 0, 0, 255 };
	inline Gradient_t DTBarIndicatorsCharged = { {106, 255, 131, 180}, {106, 255, 250, 180} };
	inline Gradient_t DTBarIndicatorsCharging = { {255, 192, 81, 180}, {255, 134, 81, 180} };
	inline Gradient_t ChokedBar =				{ { 47, 39, 0, 255 }, { 255, 210, 0, 255 } };
	inline Gradient_t GradientHealthBar =				{ { 255, 0, 0, 255 }, { 0, 202, 124, 255 } };
	inline Gradient_t GradientOverhealBar =		{ { 0, 202, 124, 255 }, { 167, 255, 237, 255 } };
	inline Gradient_t UberchargeBar =			{ { 255, 255, 255, 255 }, { 255, 0, 228, 255 } };
	inline Color_t Cond =						{ 254, 202, 87, 255 };
	inline Color_t Target =						{ 240, 147, 43, 255 };
	inline Color_t Invuln =						{ 120, 111, 166, 255 };
	inline Color_t Cloak =						{ 165, 177, 194, 255 };
	inline Color_t Friend =						{ 32, 191, 107, 255 };
	inline Color_t Local =						{ 168, 255, 211, 255 };
	inline Color_t Ignored =					{ 32, 191, 107, 255 };
	inline Color_t Overheal =					{ 84, 160, 255, 255 };
	inline Color_t Health =						{ 0, 230, 64, 255 };
	inline Color_t Ammo =						{ 191, 191, 191, 255 };
	inline Color_t UberColor =					{ 224, 86, 253, 255 };
	inline Color_t TeamRed =					{ 255, 100, 87, 255 };
	inline Color_t TeamBlu =					{ 30, 144, 255, 255 };
	inline Color_t Enemy =						{ 255, 100, 87, 255 };
	inline Color_t rTeam =						{ 30, 144, 255, 255 };
	inline Color_t Hands =						{ 30, 144, 255, 255 };
	inline Color_t HandsOverlay =				{ 255, 127, 0, 255 };
	inline Color_t Weapon =						{ 30, 144, 255, 255 };
	inline Color_t WeaponOverlay =				{ 255, 127, 0, 255 };
	inline Color_t WorldModulation =			{ 255, 255, 255, 255 };
	inline Color_t SkyModulation =				{ 255, 255, 255, 255 };
	inline Color_t StaticPropModulation =		{ 255, 255, 255, 255 };
	inline Color_t FOVCircle =					{ 255, 255, 255, 10 };
	inline Color_t Bones =						{ 255, 255, 255, 255 };
	inline Color_t BulletTracer =				{ 255, 255, 255, 255 };
	inline Color_t FresnelBase =				{ 0,0,0,255 };
	inline Color_t FresnelBaseHands =			{ 0,0,0,255 };
	inline Color_t FresnelBaseWeps =			{ 0,0,0,255 };
	inline Color_t FresnelTop = 				{ 0,255,0,255 };
	inline Color_t AimSquareCol = 				{ 0,255,0,255 };
	inline Color_t DtOutline =					{ 30, 30, 30, 180 };
	inline Color_t NotifBG =					{ 30, 30, 30, 255 };
	inline Color_t NotifOutline =				{ 255, 101, 101, 255};
	inline Color_t NotifText =					{ 255, 255, 255, 255 };
	inline Color_t HitboxFace =					{ 255, 255, 255, 25 };
	inline Color_t HitboxEdge =					{ 255, 255, 255, 175 };
	inline Color_t WeaponIcon =					{ 255,255,255,255 };
	inline Color_t NoscopeLines1 =				{ 0,0,0,255 };
	inline Color_t NoscopeLines2 =				{ 0,0,0,100 };
	inline Color_t bonecolor =					{ 231, 95, 255, 10 };
}

namespace Utils
{
	static std::random_device RandomDevice;
	static std::mt19937 Engine{ RandomDevice() };
	__inline float RandFloatRange(float min, float max)
	{
		std::uniform_real_distribution<float> Random(min, max);
		return Random(Engine);
	}

	__inline bool W2S(const Vec3 &vOrigin, Vec3 &m_vScreen)
	{
		const matrix3x4 &worldToScreen = G::WorldToProjection.As3x4();

		float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3];
		m_vScreen.z = 0;

		if (w > 0.001)
		{
			float fl1DBw = 1 / w;
			m_vScreen.x = (g_ScreenSize.w / 2) + (0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * fl1DBw) * g_ScreenSize.w + 0.5);
			m_vScreen.y = (g_ScreenSize.h / 2) - (0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * fl1DBw) * g_ScreenSize.h + 0.5);
			return true;
		}

		return false;
	}

	__inline Color_t Rainbow(float offset = 0.f)
	{
		return
		{
			static_cast<byte>(floor(sin(I::GlobalVars->curtime + offset + 0.0f) * 127.0f + 128.0f)),
			static_cast<byte>(floor(sin(I::GlobalVars->curtime + offset + 2.0f) * 127.0f + 128.0f)),
			static_cast<byte>(floor(sin(I::GlobalVars->curtime + offset + 4.0f) * 127.0f + 128.0f)),
			255
		};
	};

	__inline void GetProjectileFireSetup(CBaseEntity *pPlayer, const Vec3 &vViewAngles, Vec3 vOffset, Vec3 *vSrc)
	{
		if (g_ConVars.cl_flipviewmodels->GetBool()) { vOffset.y *= -1.0f; }

		Vec3 vecForward = Vec3(), vecRight = Vec3(), vecUp = Vec3();
		Math::AngleVectors(vViewAngles, &vecForward, &vecRight, &vecUp);

		*vSrc = pPlayer->GetShootPos() + (vecForward * vOffset.x) + (vecRight * vOffset.y) + (vecUp * vOffset.z);
	}

	__inline bool IsGameWindowInFocus()
	{
		static HWND hwGame = 0;

		while (!hwGame) {
			hwGame = WinAPI::FindWindowW(0, _(L"Team Fortress 2"));
			return false;
		}

		return (WinAPI::GetForegroundWindow() == hwGame);
	}

	__inline void* InitKeyValue()
	{
		typedef PDWORD(__cdecl* Init_t)(int);
		static DWORD dwInitLocation = g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? 83 C4 14 85 C0 74 10 68")) + 0x1;
		static DWORD dwInit = ((*(PDWORD)(dwInitLocation)) + dwInitLocation + 4);
		static Init_t InitKeyValues = (Init_t)dwInit;

		return InitKeyValues(32);
	}

	__inline Color_t GetTeamColor(int nTeamNum, bool otherColors)
	{
		if (otherColors) {
			if (const auto& pLocal = g_EntityCache.GetLocal()) {
				// Enemy/Team based colors
				auto lPlayerTeam = pLocal->GetTeamNum();
				if (lPlayerTeam == 2 && nTeamNum == 2) return Colors::rTeam;
				else if (lPlayerTeam == 3 && nTeamNum == 3) return Colors::rTeam;
				else if (lPlayerTeam == 2 && nTeamNum == 3) return Colors::Enemy;
				else if (lPlayerTeam == 3 && nTeamNum == 2) return Colors::Enemy;
				else return Colors::White;
			}
		}
		else {
			switch (nTeamNum)
			{
			case 2: return Colors::TeamRed;
			case 3: return Colors::TeamBlu;
			default: return Colors::White;
			}
		}
		return Colors::White;
	}

	__inline Color_t GetEntityDrawColor(CBaseEntity* pEntity, bool enableOtherColors)
	{
		Color_t out = GetTeamColor(pEntity->GetTeamNum(), enableOtherColors);
		PlayerInfo_t info{}; I::EngineClient->GetPlayerInfo(pEntity->GetIndex(), &info);

		if (pEntity->IsPlayer())
		{
			if (g_EntityCache.GetLocal()->GetIndex() == pEntity->GetIndex())
				out = Colors::Local;

			else if (g_EntityCache.IsFriend(pEntity->GetIndex()) || pEntity == g_EntityCache.GetLocal())
				out = Colors::Friend;

			else if (G::IsIgnored(info.friendsID))
				out = Colors::Ignored;

			else if (pEntity->IsCloaked())
				out = Colors::Cloak;

			else if (!pEntity->IsVulnerable())
				out = Colors::Invuln;
		}

		if (pEntity->GetIndex() == G::CurrentTargetIdx)
			out = Colors::Target;

		return out;
	}

	__inline const char* GetClassByIndex(const int nClass)
	{
		static const char* szClasses[] = { "unknown", "scout", "sniper", "soldier", "demoman",
										   "medic",   "heavy", "pyro",   "spy",     "engineer" };

		return (nClass < 10 && nClass > 0) ? szClasses[nClass] : szClasses[0];
	}

	__inline Color_t GetHealthColor(int nHP, int nMAXHP)
	{
		nHP = std::max(0, std::min(nHP, nMAXHP));
		int r = std::min((510 * (nMAXHP - nHP)) / nMAXHP, 200);
		int g = std::min((510 * nHP) / nMAXHP, 200);
		return { static_cast<byte>(r), static_cast<byte>(g), 0, 255 };
	}

	__inline bool IsOnScreen(CBaseEntity* pLocal, CBaseEntity* pEntity)
	{
		Vec3 WSC = pEntity->GetWorldSpaceCenter();

		if (WSC.DistTo(pLocal->GetWorldSpaceCenter()) > 300.0f)
		{
			Vec3 vScreen = {};

			if (Utils::W2S(pEntity->GetWorldSpaceCenter(), vScreen))
			{
				if (vScreen.x < -400
					|| vScreen.x > g_ScreenSize.w + 400
					|| vScreen.y < -400
					|| vScreen.y > g_ScreenSize.h + 400)
					return false;
			}

			else return false;
		}

		return true;
	}

	__inline void TraceHull(const Vec3 &vecStart, const Vec3 &vecEnd, const Vec3 &vecHullMin, const Vec3 &vecHullMax,
		unsigned int nMask, CTraceFilter *pFilter, CGameTrace *pTrace)
	{
		Ray_t ray;
		ray.Init(vecStart, vecEnd, vecHullMin, vecHullMax);
		I::EngineTrace->TraceRay(ray, nMask, pFilter, pTrace);
	}

	__inline void Trace(const Vec3 &vecStart, const Vec3 &vecEnd, unsigned int nMask, CTraceFilter *pFilter, CGameTrace *pTrace)
	{
		Ray_t ray;
		ray.Init(vecStart, vecEnd);
		I::EngineTrace->TraceRay(ray, nMask, pFilter, pTrace);
	}

	__inline int RandInt(int min, int max)
	{
		//This allows us to reach closer to true randoms generated
		//I don't think we need to update the seed more than once
		static const unsigned nSeed = std::chrono::system_clock::now().time_since_epoch().count();

		std::default_random_engine gen(nSeed);
		std::uniform_int_distribution<int> distr(min, max);
		return distr(gen);
	}

	__inline int RandIntSimple(int min, int max)
	{
		std::random_device rd; std::mt19937 gen(rd()); std::uniform_int_distribution<> distr(min, max);
		return distr(gen);
	}

	__inline void FixMovement(CUserCmd *pCmd, const Vec3 &vecTargetAngle)
	{
		Vec3 vecMove(pCmd->forwardmove, pCmd->sidemove, pCmd->upmove);
		Vec3 vecMoveAng = Vec3();

		Math::VectorAngles(vecMove, vecMoveAng);

		float fSpeed = Math::FastSqrt(vecMove.x * vecMove.x + vecMove.y * vecMove.y);
		float fYaw = DEG2RAD(vecTargetAngle.y - pCmd->viewangles.y + vecMoveAng.y);

		pCmd->forwardmove = (cos(fYaw) * fSpeed);
		pCmd->sidemove = (sin(fYaw) * fSpeed);
	}

	__inline int UnicodeToUTF8(const wchar_t* unicode, char* ansi, int ansiBufferSize)
	{
		int result = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, ansi, ansiBufferSize, NULL, NULL);
		ansi[ansiBufferSize - 1] = 0;
		return result;
	}

	__inline int UTF8ToUnicode(const char* ansi, wchar_t* unicode, int unicodeBufferSizeInBytes)
	{
		int chars = MultiByteToWideChar(CP_UTF8, 0, ansi, -1, unicode, unicodeBufferSizeInBytes / sizeof(wchar_t));
		unicode[(unicodeBufferSizeInBytes / sizeof(wchar_t)) - 1] = 0;
		return chars;
	}

	__inline std::wstring ConvertUtf8ToWide(const std::string_view& str)
	{
		int count = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), NULL, 0);
		std::wstring wstr(count, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), &wstr[0], count);
		return wstr;
	}

	__inline float ATTRIB_HOOK_FLOAT(float base_value, const char *search_string, CBaseEntity *ent, void *buffer, bool is_global_const_string)
	{
		static auto fn = reinterpret_cast<float(__cdecl *)(float, const char *, CBaseEntity *, void *, bool)>(g_Pattern.Find(_(L"client.dll"),
			_(L"55 8B EC 83 EC 0C 8B 0D ? ? ? ? 53 56 57 33 F6 33 FF 89 75 F4 89 7D F8 8B 41 08 85 C0 74 38")));

		return fn(base_value, search_string, ent, buffer, is_global_const_string);
	}

	__inline void RemovePEH(HINSTANCE hinstDLL)
	{
		DWORD dwProtect = 0x0;
		auto pImageDOS = reinterpret_cast<PIMAGE_DOS_HEADER>(hinstDLL);

		if (pImageDOS->e_magic == IMAGE_DOS_SIGNATURE)
		{
			auto pImageNT = reinterpret_cast<PIMAGE_NT_HEADERS>(pImageDOS + pImageDOS->e_lfanew);
			const auto SizeOfPE = pImageNT->FileHeader.SizeOfOptionalHeader;

			if (pImageNT->Signature == IMAGE_NT_SIGNATURE && SizeOfPE)
			{
				const auto HeaderSize = static_cast<size_t>(SizeOfPE);
				VirtualProtect(reinterpret_cast<LPVOID>(hinstDLL), HeaderSize, PAGE_EXECUTE_READWRITE, &dwProtect);
				RtlZeroMemory(reinterpret_cast<LPVOID>(hinstDLL), HeaderSize);
				VirtualProtect(reinterpret_cast<LPVOID>(hinstDLL), HeaderSize, dwProtect, &dwProtect);
			}
		}
	}

	__inline int SeedFileLineHash(int seedvalue, const char *sharedname, int additionalSeed)
	{
		CRC32_t retval;

		CRC32_Init(&retval);

		CRC32_ProcessBuffer(&retval, (void *)&seedvalue, sizeof(int));
		CRC32_ProcessBuffer(&retval, (void *)&additionalSeed, sizeof(int));
		CRC32_ProcessBuffer(&retval, (void *)sharedname, strlen(sharedname));

		CRC32_Final(&retval);

		return (int)(retval);
	}

	__inline int SharedRandomInt(unsigned iseed, const char *sharedname, int iMinVal, int iMaxVal, int additionalSeed)
	{
		int seed = SeedFileLineHash(iseed, sharedname, additionalSeed);
		I::UniformRandomStream->SetSeed(seed);
		return I::UniformRandomStream->RandomInt(iMinVal, iMaxVal);
	}

	__inline void RandomSeed(int iSeed)
	{
		static auto RandomSeedFn = reinterpret_cast<void(*)(uint32_t)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));
		RandomSeedFn(iSeed);
	}

	__inline float RandomFloat(float flMinVal = 0.0f, float flMaxVal = 1.0f)
	{
		static auto RandomFloatFn = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));
		return RandomFloatFn(flMinVal, flMaxVal);
	}

	__inline bool VisPos(CBaseEntity *pSkip, CBaseEntity *pEntity, const Vec3 &from, const Vec3 &to)
	{
		CGameTrace trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		Trace(from, to, (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);
		return ((trace.entity && trace.entity == pEntity) || trace.flFraction > 0.99f);
	}

	__inline bool VisPosHitboxId(CBaseEntity *pSkip, CBaseEntity *pEntity, const Vec3 &from, const Vec3 &to, int nHitbox)
	{
		CGameTrace trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		Trace(from, to, (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);
		return (trace.entity && trace.entity == pEntity && trace.hitbox == nHitbox);
	}

	__inline bool VisPosHitboxIdOut(CBaseEntity *pSkip, CBaseEntity *pEntity, const Vec3 &from, const Vec3 &to, int &nHitboxOut)
	{
		CGameTrace trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		Trace(from, to, (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);

		if (trace.entity && trace.entity == pEntity) {
			nHitboxOut = trace.hitbox;
			return true;
		}

		return false;
	}

	__inline bool VisPosFraction(CBaseEntity *pSkip, const Vec3 &from, const Vec3 &to)
	{
		CGameTrace trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		Trace(from, to, (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);
		return (trace.flFraction > 0.99f);
	}

	__inline bool isFeigningDeath(CBaseEntity* pQuery) {
		CTFWeaponInvis* slot4 = reinterpret_cast<CTFWeaponInvis*>(pQuery->GetWeaponFromSlot(4));
		if (slot4) {
			if (slot4->GetWeaponID() == TF_WEAPON_INVIS && slot4->HasFeignDeath()) {
				if (pQuery->GetFeignDeathReady()) {
					return true;
				}
			}
		}
		return false;
	}

	__inline EWeaponType GetWeaponType(CBaseCombatWeapon *pWeapon)
	{
		if (!pWeapon)
			return EWeaponType::UNKNOWN;

		if (pWeapon->GetSlot() == EWeaponSlots::SLOT_MELEE)
			return EWeaponType::MELEE;

		switch (pWeapon->GetWeaponID())
		{
			case TF_WEAPON_ROCKETLAUNCHER:
			case TF_WEAPON_FLAME_BALL:
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_FLAREGUN:
			case TF_WEAPON_COMPOUND_BOW:
			case TF_WEAPON_DIRECTHIT:
			case TF_WEAPON_CROSSBOW:
			case TF_WEAPON_PARTICLE_CANNON:
			case TF_WEAPON_DRG_POMSON:
			case TF_WEAPON_RAYGUN_REVENGE:
			case TF_WEAPON_RAYGUN:
			case TF_WEAPON_CANNON:
			case TF_WEAPON_SYRINGEGUN_MEDIC:
			case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
			case TF_WEAPON_FLAMETHROWER:
			case TF_WEAPON_CLEAVER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			{
				return EWeaponType::PROJECTILE;
			}

			default:
			{
				int nDamageType = pWeapon->GetDamageType();

				if (nDamageType & DMG_BULLET || nDamageType && DMG_BUCKSHOT)
					return EWeaponType::HITSCAN;

				break;
			}
		}

		if (G::CurItemDefIndex == Heavy_s_RoboSandvich ||
			G::CurItemDefIndex == Heavy_s_Sandvich ||
			G::CurItemDefIndex == Heavy_s_FestiveSandvich ||
			G::CurItemDefIndex == Heavy_s_Fishcake ||
			G::CurItemDefIndex == Heavy_s_TheDalokohsBar ||
			G::CurItemDefIndex == Heavy_s_SecondBanana) {
			return EWeaponType::HITSCAN;
		}

		return EWeaponType::UNKNOWN;
	}

	__inline uintptr_t GetVirtual(void* pBaseClass, unsigned int nIndex)
	{
		return static_cast<uintptr_t>((*static_cast<int**>(pBaseClass))[nIndex]);
	}

	__inline bool IsAttacking(CUserCmd *pCmd, CBaseCombatWeapon *pWeapon)
	{
		if (pWeapon->GetSlot() == SLOT_MELEE)
		{
			if (pWeapon->GetWeaponID() == TF_WEAPON_KNIFE)
				return ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack);

			else return fabs(pWeapon->GetSmackTime() - I::GlobalVars->curtime) < I::GlobalVars->interval_per_tick * 2.0f;
		}

		else
		{
			if (G::CurItemDefIndex == Soldier_m_TheBeggarsBazooka)
			{
				static bool bLoading = false;

				if (pWeapon->GetClip1() > 0)
					bLoading = true;

				if (!(pCmd->buttons & IN_ATTACK) && bLoading) {
					bLoading = false;
					return true;
				}
			}

			else
			{
				int ID = pWeapon->GetWeaponID();
				switch (ID) {
				case TF_WEAPON_COMPOUND_BOW:
				case TF_WEAPON_PIPEBOMBLAUNCHER:
				case TF_WEAPON_STICKY_BALL_LAUNCHER:
				case TF_WEAPON_GRENADE_STICKY_BALL:
				case TF_WEAPON_CANNON:
				{
					static bool bCharging = false;

					if (pWeapon->GetChargeBeginTime() > 0.0f)
						bCharging = true;

					if (!(pCmd->buttons & IN_ATTACK) && bCharging) {
						bCharging = false;
						return true;
					}
					break;
				}
				case TF_WEAPON_JAR:
				case TF_WEAPON_JAR_MILK:
				case TF_WEAPON_JAR_GAS:
				case TF_WEAPON_GRENADE_JAR_GAS:
				case TF_WEAPON_CLEAVER:
				{
					static float flThrowTime = 0.0f;

					if ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack && !flThrowTime)
						flThrowTime = I::GlobalVars->curtime + 0.16f;

					if (flThrowTime && I::GlobalVars->curtime >= flThrowTime) {
						flThrowTime = 0.0f;
						return true;
					}
					break;
				}
				default:
				{
					if ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack)
					{ return true; }
					break;
				}
				}
			}
		}

		return false;
	}

	__inline Vector ComputeMove(const CUserCmd* pCmd, CBaseEntity* pLocal, Vec3& a, Vec3& b)
	{
		Vec3 diff = (b - a);
		if (diff.Length() == 0.0f)
			return Vec3(0.0f, 0.0f, 0.0f);
		const float x = diff.x;
		const float y = diff.y;
		Vec3 vsilent(x, y, 0);
		Vec3 ang;
		Math::VectorAngles(vsilent, ang);
		float yaw = DEG2RAD(ang.y - pCmd->viewangles.y);
		float pitch = DEG2RAD(ang.x - pCmd->viewangles.x);
		Vec3 move = { cos(yaw) * 450.0f, -sin(yaw) * 450.0f, -cos(pitch) * 450.0f };

		// Only apply upmove in water
		if (!(I::EngineTrace->GetPointContents(pLocal->GetEyePosition()) & CONTENTS_WATER))
			move.z = pCmd->upmove;
		return move;
	}

	__inline void WalkTo(CUserCmd* pCmd, CBaseEntity* pLocal, Vec3& a, Vec3& b, float scale)
	{
		// Calculate how to get to a vector
		auto result = ComputeMove(pCmd, pLocal, a, b);
		// Push our move to usercmd
		pCmd->forwardmove = result.x * scale;
		pCmd->sidemove = result.y * scale;
		pCmd->upmove = result.z * scale;
	}

	__inline void StopMovement(CUserCmd* pCmd, bool Safe = true) {
		if (Safe && G::IsAttacking) { return; }
		if (CBaseEntity* pLocal = g_EntityCache.GetLocal()) {
			const float direction = Math::VelocityToAngles(pLocal->m_vecVelocity()).y;
			pCmd->viewangles.x = 90;
			pCmd->viewangles.y = direction;
			pCmd->viewangles.z = 0;
			pCmd->sidemove = 0; pCmd->forwardmove = 0;
			G::ShouldStop = false;
		}
	}

	__inline void WalkTo(CUserCmd* pCmd, CBaseEntity* pLocal, Vec3& pDestination)
	{
		Vec3 localPos = pLocal->GetVecOrigin();
		WalkTo(pCmd, pLocal, localPos, pDestination, 1.f);
	}

	__inline void BlockMovement(CUserCmd* pCmd)
	{
		pCmd->forwardmove = 0.f;
		pCmd->sidemove = 0.f;
		pCmd->upmove = 0.f;
	}

	__inline int HandleToIDX(int pHandle)
	{
		return pHandle & 0xFFF;
	}

	// A function to find a weapon by WeaponID
	__inline int GetWeaponByID(CBaseEntity* pPlayer, int pWeaponID)
	{
		// Invalid player
		if (!pPlayer) { return -1; }

		const size_t* hWeapons = pPlayer->GetMyWeapons();
		// Go through the handle array and search for the item
		for (int i = 0; hWeapons[i]; i++)
		{
			if (!(HandleToIDX(hWeapons[i]) >= 0 && HandleToIDX(hWeapons[i]) <= 2049 && HandleToIDX(hWeapons[i]) < 2048))
			{
				continue;
			}
			// Get the weapon
			auto* weapon = reinterpret_cast<CBaseCombatWeapon*>(I::ClientEntityList->GetClientEntityFromHandle(HandleToIDX(hWeapons[i])));
			// if weapon is what we are looking for, return true
			if (weapon && weapon->GetWeaponID() == pWeaponID)
			{
				return weapon->GetIndex();
			}
		}
		// Nothing found
		return -1;
	}

	// Returns the teleporter exit of a given owner
	__inline bool GetTeleporterExit(int ownerIdx, Vec3* out)
	{
		const auto& buildings = g_EntityCache.GetGroup(EGroupType::BUILDINGS_ALL);

		for (const auto& pBuilding : buildings)
		{
			if (!pBuilding->IsAlive()) { continue; }

			const auto& building = reinterpret_cast<CBaseObject*>(pBuilding);
			const auto nType = static_cast<EBuildingType>(building->GetType());

			if (nType == EBuildingType::TELEPORTER && building->GetObjectMode() == 1 && building->GetOwner()->GetIndex() == ownerIdx)
			{
				*out = building->GetAbsOrigin();
				return true;
			}
		}

		return false;
	}

	__inline int GetPlayerForUserID(int userID)
	{
		for (int i = 1; i <= I::EngineClient->GetMaxClients(); i++)
		{
			PlayerInfo_t playerInfo{};
			if (!I::EngineClient->GetPlayerInfo(i, &playerInfo))
			{
				continue;
			}
			// Found player
			if (playerInfo.userID == userID)
			{
				return i;
			}
		}
		return 0;
	}

	__inline bool WillProjectileHit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& targetPos)
	{
		CGameTrace trace{};
		static CTraceFilterWorldAndPropsOnly traceFilter = {};
		traceFilter.pSkip = pLocal;

		Vec3 shootPos = pLocal->GetEyePosition();
		const Vec3 aimAngles = Math::CalcAngle(shootPos, targetPos);

		{
			switch (pWeapon->GetWeaponID())
			{
			case TF_WEAPON_RAYGUN_REVENGE:
			case TF_WEAPON_ROCKETLAUNCHER:
			case TF_WEAPON_DIRECTHIT:
			{
				Vec3 vecOffset(23.5f, 12.0f, -3.0f); //tf_weaponbase_gun.cpp @L529 & @L760
				if (pLocal->IsDucking())
				{
					vecOffset.z = 8.0f;
				}
				GetProjectileFireSetup(pLocal, aimAngles, vecOffset, &shootPos);
				break;
			}
			case TF_WEAPON_SYRINGEGUN_MEDIC:
			{
				const Vec3 vecOffset(16.f, 6.f, -8.f); //tf_weaponbase_gun.cpp @L628
				GetProjectileFireSetup(pLocal, aimAngles, vecOffset, &shootPos);
				break;
			}
			case TF_WEAPON_COMPOUND_BOW:
			{
				const Vec3 vecOffset(23.5f, 12.0f, -3.0f); //tf_weapon_grapplinghook.cpp @L355 ??
				GetProjectileFireSetup(pLocal, aimAngles, vecOffset, &shootPos);
				break;
			}
			case TF_WEAPON_RAYGUN:
			case TF_WEAPON_PARTICLE_CANNON:
			case TF_WEAPON_DRG_POMSON:
			{
				Vec3 vecOffset(23.5f, -8.0f, -3.0f); //tf_weaponbase_gun.cpp @L568
				if (pLocal->IsDucking())
				{
					vecOffset.z = 8.0f;
				}
				GetProjectileFireSetup(pLocal, aimAngles, vecOffset, &shootPos);
				break;
			}
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_STICKBOMB:
			case TF_WEAPON_STICKY_BALL_LAUNCHER:
			{
				// TODO: Implement this
				break;
			}
			default: break;
			}
		}

		TraceHull(shootPos, targetPos, Vec3(-3.8f, -3.8f, -3.8f), Vec3(3.8f, 3.8f, 3.8f), MASK_SHOT_HULL, &traceFilter, &trace);
		return !trace.DidHit();
	}
}

namespace Particles {
	inline void DispatchEffect(const char* pName, const CEffectData& data)
	{
		using DispatchEffect_Fn = int(__cdecl*)(const char*, const CEffectData&);
		static DWORD a = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 83 C4 08 EB 15") + 0x1;
		static DWORD b = ((*(PDWORD)(a)) + a + 4);
		static DispatchEffect_Fn fn = (DispatchEffect_Fn)b;

		fn(pName, data);
	}

	inline int GetParticleSystemIndex(const char* pParticleSystemName)
	{
		using GetParticleSystemIndex_Fn = int(__cdecl*)(const char*);
		static DWORD a = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? D9 EE 83 C4 04 50") + 0x1;
		static DWORD b = ((*(PDWORD)(a)) + a + 4);
		static GetParticleSystemIndex_Fn fn = (GetParticleSystemIndex_Fn)b;

		return fn(pParticleSystemName);
	}

	inline void DispatchParticleEffect(int iEffectIndex, Vector vecOrigin, Vector vecStart, Vector vecAngles, CBaseEntity* pEntity)
	{
		//E8 ? ? ? ? 83 C4 2C F6 46 30 01

		CEffectData data;
		data.m_nHitBox = iEffectIndex;
		data.m_vOrigin = vecOrigin;
		data.m_vStart = vecStart;
		data.m_vAngles = vecAngles;

		if (pEntity) {
			data.m_nEntIndex = pEntity->GetIndex();
			data.m_fFlags |= (1 << 0);
			data.m_nDamageType = 2;
		}
		else {
			data.m_nEntIndex = 0;
		}

		data.m_bCustomColors = true;

		DispatchEffect("ParticleEffect", data);
	}


	inline void DispatchParticleEffect(const char* pszParticleName, Vec3 vecOrigin, Vec3 vecAngles, CBaseEntity* pEntity = 0);
	inline void DispatchParticleEffect(const char* pszParticleName, Vec3 vecOrigin, Vec3 vecAngles, CBaseEntity* pEntity)
	{
		//E8 ? ? ? ? 83 C4 20 8D 4D CC

		int iIndex = GetParticleSystemIndex(pszParticleName);
		DispatchParticleEffect(iIndex, vecOrigin, vecOrigin, vecAngles, pEntity);
	}


	inline void ParticleTracer(const char* pszTracerEffectName, const Vector& vecStart, const Vector& vecEnd, int iEntIndex, int iAttachment, bool bWhiz) {
		using ParticleTracerFn = void(__cdecl*)(const char*, const Vec3&, const Vec3&, int, int, bool);
		static auto UTIL_ParticleTracer = reinterpret_cast<ParticleTracerFn>(g_Pattern.Find(L"client.dll", _(L"55 8B EC FF 75 08 E8 ? ? ? ? D9 EE 83")));
		UTIL_ParticleTracer(pszTracerEffectName, vecStart, vecEnd, iEntIndex, iAttachment, bWhiz);

	}
}