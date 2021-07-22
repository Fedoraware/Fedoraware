#include "ChatInfo.h"

#include "../Vars.h"

#define GET_PLAYER_USERID(userid) g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(userid))
#define GET_INDEX_USERID(userid) g_Interfaces.Engine->GetPlayerForUserID(userid)

void CChatInfo::AddListeners()
{
	//Client
	g_Interfaces.GameEvent->AddListener(this, _("player_changeclass"), false);
	g_Interfaces.GameEvent->AddListener(this, _("flagstatus_update"), false);

	//Server
	g_Interfaces.GameEvent->AddListener(this, _("player_activate"), true);
	g_Interfaces.GameEvent->AddListener(this, _("player_disconnect"), true);
	g_Interfaces.GameEvent->AddListener(this, _("player_connect"), true);
}

void CChatInfo::RemoveListeners()
{
	g_Interfaces.GameEvent->RemoveListener(this);
}

void CChatInfo::FireGameEvent(CGameEvent* pEvent)
{
	const auto& pLocal = g_EntityCache.m_pLocal;

	if (pEvent)
	{
		const int nLocal = g_Interfaces.Engine->GetLocalPlayer();
		const std::string_view szEvent(pEvent->GetName());

		if (pLocal && Vars::Visuals::ChatInfo.m_Var)
		{
			int nLocalTeam = pLocal->GetTeamNum();

			if (!szEvent.compare(_("player_changeclass")))
			{
				if (const auto& pEntity = GET_PLAYER_USERID(pEvent->GetInt(_("userid"))))
				{
					if (pEntity->GetTeamNum() != nLocalTeam)
					{
						int nIndex = pEntity->GetIndex();

						PlayerInfo_t pi;
						if (g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi))
						{
							char szBuff[85];
							sprintf(szBuff, _("\x3%s is now %s"), pi.name, Utils::GetClassByIndex(pEvent->GetInt(XorStr("class").c_str())));
							g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nIndex, szBuff);
						}
					}
				}

				return;
			}

			// :(
			if (!szEvent.compare(_("player_connect")))
			{
				char szBuff[255];
				sprintf(szBuff, _("\x3%s connected. (%s)"), pEvent->GetString(_("name")), pEvent->GetString(_("address")));
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), szBuff);
				return;
			}
		}
	}
}