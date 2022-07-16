#include "Events.h"
#include "../../Features/ChatInfo/ChatInfo.h"
#include "../../Features/Resolver/Resolver.h"
#include "../../Features/AntiHack/AntiAim.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/Killstreak/Killstreak.h"

void CEventListener::Setup(const std::deque<const char*>& deqEvents)
{
	if (deqEvents.empty())
		return;

	for (auto szEvent : deqEvents) {
		I::GameEvent->AddListener(this, szEvent, false);

		if (!I::GameEvent->FindListener(this, szEvent))
			throw std::runtime_error(tfm::format("failed to add listener: %s", szEvent));
	}
}

void CEventListener::Destroy()
{
	I::GameEvent->RemoveListener(this);
}

void CEventListener::FireGameEvent(CGameEvent* pEvent) {
	if (pEvent == nullptr) { return; }

	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());
	F::ChatInfo.Event(pEvent, uNameHash);
	F::AntiAim.Event(pEvent, uNameHash);
	/*F::Killstreaker.FireEvents(pEvent, uNameHash);*/

	if (uNameHash == FNV1A::HashConst("player_hurt"))
	{
		F::Resolver.OnPlayerHurt(pEvent);
	}

	//if (uNameHash == FNV1A::HashConst("player_hurt") || uNameHash == FNV1A::HashConst("player_death")) {
	//	F::BadActors.Event(pEvent);
	//}

	// Pickup Timers
	if (Vars::Visuals::PickupTimers.Value && uNameHash == FNV1A::HashConst("item_pickup"))
	{
		const auto itemName = pEvent->GetString("item");
		if (const auto& pEntity = I::EntityList->GetClientEntity(I::Engine->GetPlayerForUserID(pEvent->GetInt("userid"))))
		{
			if (std::strstr(itemName, "medkit"))
			{
				F::Visuals.PickupDatas.push_back({ 1, I::Engine->Time(), pEntity->GetAbsOrigin() });
			}
			else if (std::strstr(itemName, "ammopack"))
			{
				F::Visuals.PickupDatas.push_back({ 0, I::Engine->Time(), pEntity->GetAbsOrigin() });
			}
		}
	}
}