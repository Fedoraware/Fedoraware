#include "AutoQueue.h"

void CAutoQueue::Run()
{
	if (!I::EngineVGui->IsGameUIVisible() || I::Engine->IsInGame()) { return; }

	if (Vars::Misc::AutoCasualQueue.Value)
	{
		static auto CTFGCClientSystem__PreInitGC = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"56 8B F1 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 68 ? ? ? ?"));
		static auto CCasualSidePanel__OnCommand = reinterpret_cast<void(__stdcall*)(const char* pCommand)>(g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 8B F9 81 FE ? ? ? ? 74 20 68 ? ? ? ? 56 E8 ? ? ? ? 83 C4 08 85 C0 74 0E 56 8B CF E8 ? ? ? ? 5F 5E 5D C2 04 00 6A 07"));
		static void* m_pSOCache = *reinterpret_cast<void**>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 85 C9 74 3D 68 ? ? ? ?") + 0x1);
		static auto CSharedObjectCache__FindBaseTypeCache = reinterpret_cast<int(__thiscall*)(void*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 8B 45 08 53 56 8B D9 89 45 F8 57 8D 45 F8 50 8D 4B 10 E8 ? ? ? ? 0F B7 F0 8B D6 3B 53 18 73 43 0F B7 43 26 B9 ? ? ? ? 66 3B C1 74 35"));
		static auto CTFParty__SpewDebug = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 68 A1 ? ? ? ? 53 8B D9 89 45 F0 8B 83 ? ? ? ? 8D 4D E8 56 8D B3 ? ? ? ? 89 5D F8 51 8B CE C7 45 ? ? ? ? ? C6 45 F4 00 FF 50 1C 8B C8 E8 ? ? ? ?"));
		static auto GTFGCClientSystem = reinterpret_cast<void* (__cdecl*)()>(g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? 0F B6 CB"));
		static auto CTFGCClientSystem__GetParty = reinterpret_cast<void* (__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8B 89 ? ? ? ? 85 C9 74 1D 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 0F 8B 48 18 85 C9 74 08 8B 40 04 8B 44 88 FC C3"));
		static auto GTFPartyClient = reinterpret_cast<void* (__cdecl*)()>(g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? FF 70 24"));
		static auto CTFPartyClient__LoadSavedCasualCriteria = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"83 79 18 00 C6 81 ? ? ? ? ? 74 06 80 79 28 00 74 07 C6 81 ? ? ? ? ? FF 35 ? ? ? ? 81 C1 ? ? ? ? E8 ? ? ? ? C3"));
		static auto CTFPartyClient__BInStandbyQueue = reinterpret_cast<bool(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8A 41 40 C3"));
		static auto CTFGCClientSystem__BHaveLiveMatch = reinterpret_cast<bool(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"56 8B F1 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74 0D 80 BE ? ? ? ? ? 75 04 B0 01 5E C3"));
		static auto CTFGCClientSystem__GetNumMatchInvites = reinterpret_cast<int(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8B 89 ? ? ? ? 85 C9 74 12 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 04 8B 40 18 C3"));

		const auto pPartyClient = GTFPartyClient();
		const auto pGameCoordinator = GTFGCClientSystem();

		if (pPartyClient && pGameCoordinator)
		{
			if (!CTFPartyClient__BInStandbyQueue(pPartyClient) &&
				!CTFGCClientSystem__BHaveLiveMatch(pGameCoordinator) &&
				CTFGCClientSystem__GetNumMatchInvites(pGameCoordinator) == 0)
			{
				CTFPartyClient__LoadSavedCasualCriteria(pPartyClient);
				CCasualSidePanel__OnCommand("find_game");
			}
		}
	}
}
