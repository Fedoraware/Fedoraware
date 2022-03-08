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
#include "DirectX/DirectX.h"
#include "../Features/Vars.h"

#define TICK_INTERVAL		( g_Interfaces.GlobalVars->interval_per_tick )
#define TIME_TO_TICKS( dt )	( static_cast<int>( 0.5f + static_cast<float>(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )	( TICK_INTERVAL * ( t ) )
#define GetKey(vKey) (Utils::IsGameWindowInFocus() && GetAsyncKeyState(vKey))
#define Q_ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))

//I for some reason have to include this here, if I don't then one steam header goes apeshit full of errors
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
	inline Color_t White =					{ 255, 255, 255, 255 };
	inline Color_t OutlineESP =				{ 0, 0, 0, 220 };
	inline Color_t Cond =					{ 254, 202, 87, 255 };
	inline Color_t Target =					{ 240, 147, 43, 255 };
	inline Color_t Invuln =					{ 120, 111, 166, 255 };
	inline Color_t Cloak =					{ 165, 177, 194, 255 };
	inline Color_t Friend =					{ 32, 191, 107, 255 };
	inline Color_t Local =					{ 168, 255, 211, 255 };
	inline Color_t Ignored =				{ 32, 191, 107, 255 };
	inline Color_t Overheal =				{ 84, 160, 255, 255 };
	inline Color_t Health =					{ 0, 230, 64, 255 };
	inline Color_t Ammo =					{ 191, 191, 191, 255 };
	inline Color_t UberColor =				{ 224, 86, 253, 255 };
	inline Color_t TeamRed =				{ 255, 100, 87, 255 };
	inline Color_t TeamBlu =				{ 30, 144, 255, 255 };
	inline Color_t Enemy =					{ 255, 100, 87, 255 };
	inline Color_t rTeam =					{ 30, 144, 255, 255 };
	inline Color_t Hands =					{ 30, 144, 255, 255 };
	inline Color_t HandsOverlay =			{ 255, 127, 0, 255 };
	inline Color_t Weapon =					{ 30, 144, 255, 255 };
	inline Color_t WeaponOverlay =			{ 255, 127, 0, 255 };
	inline Color_t WorldModulation =		{ 255, 255, 255, 255 };
	inline Color_t SkyModulation =			{ 255, 255, 255, 255 };
	inline Color_t StaticPropModulation =	{ 255, 255, 255, 255 };
	inline Color_t FOVCircle =				{ 255, 255, 255, 10 };
	inline Color_t Bones =					{ 255, 255, 255, 255 };
	inline Color_t BulletTracer =			{ 255, 255, 255, 255 };
	inline Color_t FresnelBase =			{ 0,0,0,255 };
	inline Color_t FresnelBaseHands =			{ 0,0,0,255 };
	inline Color_t FresnelBaseWeps =			{ 0,0,0,255 };
	inline Color_t FresnelTop = 			{ 0,255,0,255 };
	inline Color_t AimSquareCol = 			{ 0,255,0,255 };
	inline Color_t DtChargingLeft =			{ 255, 192, 81, 180};
	inline Color_t DtChargingRight =		{ 255, 134, 81, 180};
	inline Color_t DtChargedLeft =			{ 106, 255, 131, 180};
	inline Color_t DtChargedRight =			{ 106, 255, 250, 180 };
	inline Color_t DtOutline =				{ 30, 30, 30, 180 };
	inline Color_t NotifBG =	{ 30, 30, 30, 255 };
	inline Color_t NotifOutline =		{ 255, 101, 101, 255};
	inline Color_t NotifText =			{ 255, 255, 255, 255 };
	inline Color_t Hitbox =			{ 255, 255, 255, 0 };
	inline Color_t WeaponIcon = { 255,255,255,255 };
	inline Color_t NoscopeLines1 = { 0,0,0,255 };
	inline Color_t NoscopeLines2 = { 0,0,0,100 };
	inline Color_t bonecolor = { 231, 95, 255, 10 };
}

namespace Utils
{
	__inline IMaterial* CreateMaterial(const char* szVars)
	{
		static int nCreatedMats = 0;
		char szOut[512];
		sprintf_s(szOut, sizeof(szOut), _("SEO_material%i.vmt"), nCreatedMats++);

		char szMaterial[512];
		sprintf_s(szMaterial, sizeof(szMaterial), szVars);

		KeyValues* pVals = new KeyValues(_("matvalsxd"));
		g_KeyValUtils.Initialize(pVals, (char*)szOut);
		g_KeyValUtils.LoadFromBuffer(pVals, szOut, szMaterial);

		IMaterial* pCreated = g_Interfaces.MatSystem->Create(szOut, pVals);
		pCreated->IncrementReferenceCount();

		return pCreated;
	}

	static std::random_device RandomDevice;
	static std::mt19937 Engine{ RandomDevice() };
	__inline float RandFloatRange(float min, float max)
	{
		std::uniform_real_distribution<float> Random(min, max);
		return Random(Engine);
	}

	__inline void* CreateKeyVals(const char* szVars)
	{
		static int nCreatedKeyVals = 0;
		char szOut[512];
		sprintf_s(szOut, sizeof(szOut), _("SEO_keyvals%i.vmt"), nCreatedKeyVals++);

		KeyValues* pVals = new KeyValues(_("keyvalssmh"));
		g_KeyValUtils.Initialize(pVals, (char*)szOut);
		g_KeyValUtils.LoadFromBuffer(pVals, szOut, szVars);

		return pVals;
	}

	__inline bool W2S(const Vec3 &vOrigin, Vec3 &m_vScreen)
	{
		const matrix3x4 &worldToScreen = g_GlobalInfo.m_WorldToProjection.As3x4();

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

	__inline Color_t Rainbow()
	{
		return
		{
			static_cast<byte>(floor(sin(g_Interfaces.GlobalVars->curtime + 0.0f) * 127.0f + 128.0f)),
			static_cast<byte>(floor(sin(g_Interfaces.GlobalVars->curtime + 2.0f) * 127.0f + 128.0f)),
			static_cast<byte>(floor(sin(g_Interfaces.GlobalVars->curtime + 4.0f) * 127.0f + 128.0f)),
			255
		};
	};

	__inline void GetProjectileFireSetup(CBaseEntity *pPlayer, const Vec3 &vViewAngles, Vec3 vOffset, Vec3 *vSrc)
	{
		if (g_ConVars.cl_flipviewmodels->GetInt())
			vOffset.y *= -1.0f;

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
			if (const auto& pLocal = g_EntityCache.m_pLocal) {
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
		PlayerInfo_t info{}; g_Interfaces.Engine->GetPlayerInfo(pEntity->GetIndex(), &info);

		if (pEntity->IsPlayer())
		{
			if (g_EntityCache.m_pLocal->GetIndex() == pEntity->GetIndex())
				out = Colors::Local;

			else if (g_EntityCache.Friends[pEntity->GetIndex()] || pEntity == g_EntityCache.m_pLocal)
				out = Colors::Friend;

			else if (g_GlobalInfo.ignoredPlayers.find(info.friendsID) != g_GlobalInfo.ignoredPlayers.end())
				out = Colors::Ignored;

			else if (pEntity->IsCloaked())
				out = Colors::Cloak;

			else if (!pEntity->IsVulnerable())
				out = Colors::Invuln;
		}

		if (pEntity->GetIndex() == g_GlobalInfo.m_nCurrentTargetIdx)
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
		g_Interfaces.EngineTrace->TraceRay(ray, nMask, pFilter, pTrace);
	}

	__inline void Trace(const Vec3 &vecStart, const Vec3 &vecEnd, unsigned int nMask, CTraceFilter *pFilter, CGameTrace *pTrace)
	{
		Ray_t ray;
		ray.Init(vecStart, vecEnd);
		g_Interfaces.EngineTrace->TraceRay(ray, nMask, pFilter, pTrace);
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
		g_Interfaces.UniformRandomStream->SetSeed(seed);
		return g_Interfaces.UniformRandomStream->RandomInt(iMinVal, iMaxVal);
	}

	__inline void RandomSeed(int iSeed)
	{
		static auto RandomSeedFn = reinterpret_cast<void(__cdecl *)(int)>(reinterpret_cast<DWORD>(WinAPI::GetProcessAddr(reinterpret_cast<DWORD>(GetModuleHandleW(XorStr(L"vstdlib.dll").c_str())), XorStr("RandomSeed").c_str())));
		RandomSeedFn(iSeed);
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

	__inline EWeaponType GetWeaponType(CBaseCombatWeapon *pWeapon)
	{
		if (!pWeapon)
			return EWeaponType::UNKNOWN;

		if (pWeapon->GetSlot() == EWeaponSlots::SLOT_MELEE)
			return EWeaponType::MELEE;

		switch (pWeapon->GetWeaponID())
		{
			case TF_WEAPON_ROCKETLAUNCHER:
			case 109:
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_FLAREGUN:
			case TF_WEAPON_COMPOUND_BOW:
			case TF_WEAPON_DIRECTHIT:
			case TF_WEAPON_CROSSBOW:
			case TF_WEAPON_PARTICLE_CANNON:
			case TF_WEAPON_DRG_POMSON:
			case TF_WEAPON_RAYGUN_REVENGE:
			case TF_WEAPON_CANNON:
			case TF_WEAPON_SYRINGEGUN_MEDIC:
			case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
			case TF_WEAPON_FLAMETHROWER:
			case TF_WEAPON_CLEAVER:
			{
				return EWeaponType::PROJECTILE;
			}

			case TF_WEAPON_PIPEBOMBLAUNCHER://dragon's fury
			{
				//broken Idk
				return EWeaponType::UNKNOWN;
			}

			default:
			{
				int nDamageType = pWeapon->GetDamageType();

				if (nDamageType & DMG_BULLET || nDamageType && DMG_BUCKSHOT)
					return EWeaponType::HITSCAN;

				break;
			}
		}

		if (g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_RoboSandvich ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_Sandvich ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_FestiveSandvich ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_Fishcake ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_TheDalokohsBar ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_SecondBanana) {
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
				return ((pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_bWeaponCanAttack);

			else return fabs(pWeapon->GetSmackTime() - g_Interfaces.GlobalVars->curtime) < g_Interfaces.GlobalVars->interval_per_tick * 2.0f;
		}

		else
		{
			if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka)
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

				if (ID == TF_WEAPON_COMPOUND_BOW || ID == TF_WEAPON_PIPEBOMBLAUNCHER)
				{
					static bool bCharging = false;

					if (pWeapon->GetChargeBeginTime() > 0.0f)
						bCharging = true;

					if (!(pCmd->buttons & IN_ATTACK) && bCharging) {
						bCharging = false;
						return true;
					}
				}

				else if (ID == TF_WEAPON_JAR || ID == TF_WEAPON_JAR_MILK || ID == 107/*pyro's gas thing*/)
				{
					static float flThrowTime = 0.0f;

					if ((pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_bWeaponCanAttack && !flThrowTime)
						flThrowTime = g_Interfaces.GlobalVars->curtime + 0.16f;

					if (flThrowTime && g_Interfaces.GlobalVars->curtime >= flThrowTime) {
						flThrowTime = 0.0f;
						return true;
					}
				}

				else
				{
					if (!g_GlobalInfo.m_bWeaponCanAttack && (pCmd->buttons & IN_ATTACK))
					{
						if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN)
							pCmd->buttons |= IN_ATTACK2;

						pCmd->buttons &= ~IN_ATTACK;
					}

					if ((pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_bWeaponCanAttack)
						return true;
				}
			}
		}

		return false;
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