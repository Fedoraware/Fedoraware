#include "Events.h"
#include "../../Features/ChatInfo/ChatInfo.h"

void CEventListener::Setup(const std::deque<const char*>& deqEvents)
{
	if (deqEvents.empty())
		return;

	for (auto szEvent : deqEvents) {
		g_Interfaces.GameEvent->AddListener(this, szEvent, false);

		if (!g_Interfaces.GameEvent->FindListener(this, szEvent))
			throw std::runtime_error(tfm::format("failed to add listener: %s", szEvent));
	}
}

void CEventListener::Destroy()
{
	g_Interfaces.GameEvent->RemoveListener(this);
}

void CEventListener::FireGameEvent(CGameEvent* pEvent) {
	if (pEvent == nullptr)
		return;

	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());
	g_ChatInfo.Event(pEvent, uNameHash);
}