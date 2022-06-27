#include "AutoQueue.h"

void CAutoQueue::Run()
{
	if (!I::EngineVGui->IsGameUIVisible() || I::Engine->IsInGame()) { return; }

	// Auto queue
	if (Vars::Misc::AutoCasualQueue.Value)
	{
		static auto CCasualSidePanel__OnCommand = reinterpret_cast<void(__stdcall*)(const char* pCommand)>(g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 8B F9 81 FE ? ? ? ? 74 20 68 ? ? ? ? 56 E8 ? ? ? ? 83 C4 08 85 C0 74 0E 56 8B CF E8 ? ? ? ? 5F 5E 5D C2 04 00 6A 07"));
		static void* m_pSOCache = *reinterpret_cast<void**>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 85 C9 74 3D 68 ? ? ? ?") + 0x1);
		static auto CSharedObjectCache__FindBaseTypeCache = reinterpret_cast<int(__thiscall*)(void*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 8B 45 08 53 56 8B D9 89 45 F8 57 8D 45 F8 50 8D 4B 10 E8 ? ? ? ? 0F B7 F0 8B D6 3B 53 18 73 43 0F B7 43 26 B9 ? ? ? ? 66 3B C1 74 35"));
		static auto CTFParty__SpewDebug = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 68 A1 ? ? ? ? 53 8B D9 89 45 F0 8B 83 ? ? ? ? 8D 4D E8 56 8D B3 ? ? ? ? 89 5D F8 51 8B CE C7 45 ? ? ? ? ? C6 45 F4 00 FF 50 1C 8B C8 E8 ? ? ? ?"));

		if (!I::TFPartyClient->BInStandbyQueue() 	&&
			!I::TFGCClientSystem->BHaveLiveMatch() 	&&
			!I::TFGCClientSystem->GetNumMatchInvites())
		{
			I::TFPartyClient->LoadSavedCasualCriteria();
			CCasualSidePanel__OnCommand("find_game");
		}
	}

	// Auto accept
	if (Vars::Misc::AutoAccept.Value)
	{
		if (I::TFPartyClient->BInStandbyQueue() 	||
			!I::TFGCClientSystem->BHaveLiveMatch() 	||
			I::TFGCClientSystem->GetNumMatchInvites())
		{
			I::TFGCClientSystem->JoinMMMatch();
		}
	}

	// Join message spam | Credits to lnx00 for this cool exploit :D
	if (Vars::Misc::JoinSpam.Value)
	{
		static Timer spamTimer{ };
		if (spamTimer.Run(250))
		{
			I::TFGCClientSystem->JoinMMMatch();
		}

	}
}
