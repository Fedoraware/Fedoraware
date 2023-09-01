#include "AutoQueue.h"

void CAutoQueue::Run()
{
	const bool bInGame = (!I::EngineVGui->IsGameUIVisible() || I::EngineClient->IsInGame());

	// Auto queue
	if (Vars::Misc::AutoCasualQueue.Value == 1)
	{
		if (bInGame)
		{
			return;
		}
		const bool bInStandbyQueue = I::TFPartyClient->BInStandbyQueue();
		const bool bHaveLiveMatch = I::TFGCClientSystem->BHaveLiveMatch();
		const int nNumMatchInvites = I::TFGCClientSystem->GetNumMatchInvites();

		if (!bInStandbyQueue &&
			!bHaveLiveMatch &&
			!nNumMatchInvites)
		{
			I::TFPartyClient->LoadSavedCasualCriteria();
			I::TFPartyClient->RequestQueueForMatch(k_eTFMatchGroup_Casual_Default);
		}
	}
	if (Vars::Misc::AutoCasualQueue.Value == 2)
	{
		const bool bInQueueForMatchGroup = I::TFPartyClient->BInQueueForMatchGroup(k_eTFMatchGroup_Casual_Default);

		if (!bInQueueForMatchGroup)
		{
			I::TFPartyClient->LoadSavedCasualCriteria();
			I::TFPartyClient->RequestQueueForMatch(k_eTFMatchGroup_Casual_Default);
		}
	}

	if (bInGame)
	{
		return;
	}

	// Auto accept
	if (Vars::Misc::AntiVAC.Value)
	{
		static auto fps_max = g_ConVars.FindVar("fps_max");
		static auto host_timescale = g_ConVars.FindVar("host_timescale");
		static auto sv_cheats = g_ConVars.FindVar("sv_cheats");
		static bool lastConnect = false;

		if (I::TFGCClientSystem->GetNumMatchInvites() > 0 && !I::EngineClient->IsConnected())
		{
			sv_cheats->SetValue(1);
			fps_max->SetValue(1);
			host_timescale->SetValue(25);
		}
		else if (I::TFGCClientSystem->BHaveLiveMatch() && !I::EngineClient->IsConnected() && lastConnect)
		{
			I::TFGCClientSystem->JoinMMMatch();
		}
		else if (I::EngineClient->IsConnected() && !lastConnect)
		{
			fps_max->SetValue(0);
			host_timescale->SetValue(1);
		}

		lastConnect = I::EngineClient->IsConnected();
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
