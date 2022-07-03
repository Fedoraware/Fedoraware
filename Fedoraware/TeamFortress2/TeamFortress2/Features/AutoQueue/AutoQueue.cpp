#include "AutoQueue.h"

#include <chrono>
#include <thread>

void CAutoQueue::Run()
{
	if (!I::EngineVGui->IsGameUIVisible() || I::Engine->IsInGame()) { return; }

	// Auto queue
	if (Vars::Misc::AutoCasualQueue.Value)
	{
		if (!I::TFPartyClient->BInStandbyQueue() 	&&
			!I::TFGCClientSystem->BHaveLiveMatch() 	&&
			!I::TFGCClientSystem->GetNumMatchInvites())
		{
			I::TFPartyClient->LoadSavedCasualCriteria();
			I::TFPartyClient->RequestQueueForMatch(k_eTFMatchGroup_Casual_Default);
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
