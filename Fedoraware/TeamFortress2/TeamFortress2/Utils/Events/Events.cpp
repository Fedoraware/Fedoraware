#include "Events.h"
#include "../../Features/ChatInfo/ChatInfo.h"
#include "../../Features/Resolver/Resolver.h"
#include "../../Features/AntiHack/AntiAim.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Killstreak/Killstreak.h"
#include "../../Features/LuaEngine/Callbacks/LuaCallbacks.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Misc/Misc.h"

void CEventListener::Setup(const std::deque<const char*>& deqEvents)
{
	if (deqEvents.empty())
		return;

	for (auto szEvent : deqEvents)
	{
		I::GameEventManager->AddListener(this, szEvent, false);

		if (!I::GameEventManager->FindListener(this, szEvent))
			throw std::runtime_error(tfm::format("failed to add listener: %s", szEvent));
	}
}

void CEventListener::Destroy()
{
	I::GameEventManager->RemoveListener(this);
}

void CEventListener::FireGameEvent(CGameEvent* pEvent)
{
	if (pEvent == nullptr) { return; }
	if (I::EngineClient->IsPlayingTimeDemo()) { return; }

	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());
	F::ChatInfo.Event(pEvent, uNameHash);
	F::AntiAim.Event(pEvent, uNameHash);
	F::Statistics.Event(pEvent, uNameHash);
	/*F::Killstreaker.FireEvents(pEvent, uNameHash);*/

	if (uNameHash == FNV1A::HashConst("player_hurt"))
	{
		F::Resolver.OnPlayerHurt(pEvent);
		F::Backtrack.PlayerHurt(pEvent);
		F::BadActors.ReportDamage(pEvent);
	}

	// Run Lua callbacks
	F::LuaCallbacks.OnFireGameEvent(pEvent);

	// Pickup Timers
	if (Vars::Visuals::PickupTimers.Value && uNameHash == FNV1A::HashConst("item_pickup"))
	{
		const auto itemName = pEvent->GetString("item");
		if (const auto& pEntity = I::ClientEntityList->GetClientEntity(I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"))))
		{
			if (std::strstr(itemName, "medkit"))
			{
				F::Visuals.PickupDatas.push_back({ 1, I::EngineClient->Time(), pEntity->GetAbsOrigin() });
			}
			else if (std::strstr(itemName, "ammopack"))
			{
				F::Visuals.PickupDatas.push_back({ 0, I::EngineClient->Time(), pEntity->GetAbsOrigin() });
			}
		}
	}
}