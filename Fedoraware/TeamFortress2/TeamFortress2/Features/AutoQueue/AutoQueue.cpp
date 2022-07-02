#include "AutoQueue.h"

#include <chrono>
#include <thread>

void CAutoQueue::Run()
{
	if (!I::EngineVGui->IsGameUIVisible() || I::Engine->IsInGame()) { return; }

	// Auto queue
	if (Vars::Misc::AutoCasualQueue.Value)
	{
		static auto CCasualSidePanel_OnCommand = reinterpret_cast<void(__stdcall*)(const char* pCommand)>(g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 8B F9 81 FE ? ? ? ? 74 20 68 ? ? ? ? 56 E8 ? ? ? ? 83 C4 08 85 C0 74 0E 56 8B CF E8 ? ? ? ? 5F 5E 5D C2 04 00 6A 07"));

		if (!I::TFPartyClient->BInStandbyQueue() 	&&
			!I::TFGCClientSystem->BHaveLiveMatch() 	&&
			!I::TFGCClientSystem->GetNumMatchInvites())
		{
			I::TFPartyClient->LoadSavedCasualCriteria();
			CCasualSidePanel_OnCommand("find_game");
		}
	}

	// Auto accept
	if (Vars::Misc::AntiVAC.Value)
	{
		static auto fps_max = g_ConVars.FindVar("fps_max");
		static auto host_timescale = g_ConVars.FindVar("host_timescale");
		static auto sv_cheats = g_ConVars.FindVar("sv_cheats");
		static bool lastConnect = false;

		if (I::TFGCClientSystem->GetNumMatchInvites() > 0 && !I::Engine->IsConnected())
		{
			sv_cheats->SetValue(1);
			fps_max->SetValue(1);
			host_timescale->SetValue(25);
		} else if (I::TFGCClientSystem->BHaveLiveMatch() && !I::Engine->IsConnected() && lastConnect)
		{
			I::TFGCClientSystem->JoinMMMatch();
		} else if (I::Engine->IsConnected() && !lastConnect)
		{
			fps_max->SetValue(0);
			host_timescale->SetValue(1);
		}

		lastConnect = I::Engine->IsConnected();
	}

	// Join message spam
	if (Vars::Misc::JoinSpam.Value)
	{
		static Timer spamTimer{ };
		if (spamTimer.Run(200))
		{
			I::TFGCClientSystem->JoinMMMatch();
		}

	}
}
