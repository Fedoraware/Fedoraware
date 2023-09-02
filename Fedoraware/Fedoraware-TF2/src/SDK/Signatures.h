#pragma once
#include <format>
#include "../Utils/Pattern/Pattern.h"

constexpr auto CLIENT_DLL = "client.dll";
constexpr auto ENGINE_DLL = "engine.dll";
constexpr auto VSTDLIB_DLL = "vstdlib.dll";

class CSignature
{
	DWORD m_Address = 0;
	LPCSTR m_Module = nullptr;
	LPCSTR m_Pattern = nullptr;
	int m_Offset = 0;

	// Find and cache the address
	void Find()
	{
		m_Address = g_Pattern.Find(m_Module, m_Pattern);
		if (m_Address == 0)
		{
			const auto msg = std::format("Module: {}\nPattern: {}\nAddress: {:#x} (+{:d})", m_Module, m_Pattern, m_Address, m_Offset);
			MessageBoxA(nullptr, msg.c_str(), "Pattern not found", MB_OK | MB_ICONERROR);
		}
	}

public:
	CSignature(LPCSTR szModule, LPCSTR szPattern, int offset = 0)
		: m_Module(szModule), m_Pattern(szPattern), m_Offset(offset) { }

	// Return the address
	DWORD operator()()
	{
		if (m_Address == 0) { Find(); }
		return m_Address + m_Offset;
	}

	template <typename T> T* As() { return reinterpret_cast<T*>(this->operator()()); }
};

#define MAKE_SIGNATURE(name, module, pattern, offset) inline CSignature name{ module, pattern, offset }

// Signatures
namespace S
{
	// Interfaces
	MAKE_SIGNATURE(GlobalVars_Interface, ENGINE_DLL, "A1 ? ? ? ? 8B 11 68", 0x8);
	MAKE_SIGNATURE(ClientState_Interface, ENGINE_DLL, "68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3", 0x1);
	MAKE_SIGNATURE(DemoPlayer_Interface, ENGINE_DLL, "8B 0D ? ? ? ? 85 C9 74 3B 8B 01 8B 40 18 FF D0 84 C0 74 30", 0x2);
	MAKE_SIGNATURE(TFGCClientSystem_Interface, CLIENT_DLL, "B9 ? ? ? ? 50 E8 ? ? ? ? 8B 5D F8", 0x1);
	MAKE_SIGNATURE(TFInventoryManager_Interface, CLIENT_DLL, "B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ?", 0x1);
	MAKE_SIGNATURE(Input_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 FF 50 24 8B 45 FC", 0x2);
	MAKE_SIGNATURE(UniformRandomStream_Interface, VSTDLIB_DLL, "B9 ? ? ? ? 85 C0 0F 45 C8 89 0D ? ? ? ? 5D C3", 0x1);
	MAKE_SIGNATURE(ViewRenderBeams_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 FF 50 18 0F B7 96 ? ? ? ?", 0x2);
	MAKE_SIGNATURE(TFGameRules_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 8B 80 ? ? ? ? FF D0 84 C0 0F 84 ? ? ? ? 80 BB ? ? ? ? ?", 0x1);
	MAKE_SIGNATURE(ThirdPersonManager_Interface, CLIENT_DLL, "B9 ? ? ? ? E8 ? ? ? ? 84 C0 74 42 8B 86", 0x1);
	MAKE_SIGNATURE(ClientModeTFNormal_Interface, CLIENT_DLL, "B9 ? ? ? ? A3 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 83 C4 04 8B 35 ? ? ? ?", 0x1);
	MAKE_SIGNATURE(HostState_Interface, ENGINE_DLL, "8B 15 ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85", 0x1);

	MAKE_SIGNATURE(DirectXDevice, "shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C", 0x1);
	MAKE_SIGNATURE(ClientTable, CLIENT_DLL, "8B 0D ? ? ? ? 8B 02 D9 05", 0x0);

	// Hooks
	MAKE_SIGNATURE(C_BaseAnimating_FrameAdvance, CLIENT_DLL, "55 8B EC 83 EC 14 56 8B F1 57 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 83 BE ? ? ? ? ? 75 05 E8 ? ? ? ? 8B BE ? ? ? ? 85 FF 0F 84 ? ? ? ? F3 0F 10 45 ? 0F 57 D2 A1 ? ? ? ? 0F 2E C2 F3 0F 10 48 ? F3 0F 11 4D", 0x0);
	MAKE_SIGNATURE(C_BaseAnimating_Interpolate, CLIENT_DLL, "55 8B EC 83 EC 2C 56 8B F1 83 BE ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 86 ? ? ? ? 57 33 FF F3 0F 11 45 ? 80 BE ? ? ? ? ? 75 13 FF B6 ? ? ? ? E8 ? ? ? ? 8B 8E ? ? ? ? 88 01 8D 45 FC 8B CE 50", 0x0);
	MAKE_SIGNATURE(C_BaseAnimating_MaintainSequenceTransitions, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B 75 ? 57 8B F9 8B CE E8 ? ? ? ? 85 C0", 0x0);
	MAKE_SIGNATURE(C_BaseCombatWeapon_AddToCritBucket, CLIENT_DLL, "55 8B EC A1 ? ? ? ? F3 0F 10 81 ? ? ? ? F3 0F 10 48 ? 0F 2F C8 76 1D F3 0F 58 45 ? 0F 2F C8 F3 0F 11 81 ? ? ? ? 77 03 0F 28 C1 F3 0F 11 81 ? ? ? ? 5D C2 04 00", 0x0);
	MAKE_SIGNATURE(C_BaseCombatWeapon_IsAllowedToWithdrawFromCritBucket, CLIENT_DLL, "55 8B EC 56 8B F1 0F B7 86 ? ? ? ? FF 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 80 B8 ? ? ? ? ? 74 0A F3 0F 10 15", 0x0);
	MAKE_SIGNATURE(C_BaseEntity_FireBullets, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 53 56 57 8B F9 8B 5D", 0x0);
	MAKE_SIGNATURE(C_BaseEntity_Interpolate, CLIENT_DLL, "55 8B EC 83 EC 28 53 57 8D 45 FC 8B F9 50 8D 45 F0 50 8D 45 E4 50 8D 45 D8 50 8D 45 08 50 E8 ? ? ? ? 83 7D FC 00 8B D8 74 46 0F B7 8F ? ? ? ? B8 ? ? ? ? 66 3B C8", 0x0);
	MAKE_SIGNATURE(C_BaseEntity_SetAbsVelocity, CLIENT_DLL, "55 8B EC 83 EC ? 56 57 8B 7D ? 8B F1 F3 0F", 0x0);
	MAKE_SIGNATURE(C_BasePlayer_CalcViewModelView, CLIENT_DLL, "55 8B EC 83 EC 70 8B 55 0C 53 8B 5D 08 89 4D FC 8B 02 89 45 E8 8B 42 04 89 45 EC 8B 42 08 89 45 F0 56 57", 0x0);
	MAKE_SIGNATURE(C_BaseViewModel_ShouldFlipViewModel, CLIENT_DLL, "8B 89 ? ? ? ? 85 C9 74 ? BA ? ? ? ? 83 F9 ? 74 ? 0F B7 D1 A1 ? ? ? ? C1 E2 ? 83 C0 ? 03 C2 74 ? C1 E9 ? 39 48 ? 75 ? 8B 08 85 C9 74 ? A1", 0x0);
	MAKE_SIGNATURE(C_OP_RenderSprites_RenderSpriteCard, CLIENT_DLL, "55 8B EC 83 EC 28 56 8B 75 10 57 8B 7D 14 8B C7 99 83 E7 03 83 E2 03", 0x0);
	MAKE_SIGNATURE(C_TFPlayer_AvoidPlayers, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 57 8B F9 89 7D D4 83 78 30 00", 0x0);
	MAKE_SIGNATURE(C_TFPlayer_FireEvent, CLIENT_DLL, "55 8B EC 8B 45 ? 81 EC ? ? ? ? 53 56 57 8B F9 3D", 0x0);
	MAKE_SIGNATURE(C_TFRagdoll_CreateTFRagdoll, CLIENT_DLL, "55 8B EC B8 ? ? ? ? E8 ? ? ? ? 53 56 57 8B F9 8B 8F ? ? ? ? 85 C9 0F 85", 0x0);
	MAKE_SIGNATURE(C_TFWeaponBase_CalcIsAttackCritical, CLIENT_DLL, "53 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B D8 83 C4 14 85 DB 0F 84", 0x0);

	// Values
	MAKE_SIGNATURE(RandomSeed, CLIENT_DLL, "C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34", 0x2);
	MAKE_SIGNATURE(AllowSecureServers, ENGINE_DLL, "C6 05 ? ? ? ? ? 8A C3", 0x2);
}
