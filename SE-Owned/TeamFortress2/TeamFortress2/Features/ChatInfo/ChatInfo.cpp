#include "ChatInfo.h"

#include "../Vars.h"
#include "../Misc/Misc.h"

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
	g_Interfaces.GameEvent->AddListener(this, _("vote_cast"), true);
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
		g_Misc.VoteRevealer(*pEvent);

		const int nLocal = g_Interfaces.Engine->GetLocalPlayer();
		const std::string_view szEvent(pEvent->GetName());

		if (pLocal && Vars::Visuals::ChatInfo.m_Var)
		{
			int nLocalTeam = pLocal->GetTeamNum();
			const char* CathookName = pEvent->GetName();
			int CathookLine = pEvent->GetInt("line", -1);
			int CathookPanel = pEvent->GetInt("panel", -1);
			float CathookX = pEvent->GetFloat("x", -1.f);
			float CathookY = pEvent->GetFloat("y", -1.f);

			if (strstr(CathookName, "cl_drawline"))
				if (CathookLine == 0 && CathookPanel == 2)
					if ((CathookX == 0xCA8 || CathookX == 0xCA7) && CathookY == 1234567.f)
						g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, "yep i got one");

			if (!szEvent.compare(_("player_changeclass")))
			{
				if (const auto& pEntity = GET_PLAYER_USERID(pEvent->GetInt(_("userid"))))
				{

					/*if (pEntity->GetTeamNum() != nLocalTeam)
					{*/
						int nIndex = pEntity->GetIndex();

						PlayerInfo_t pi;
						if (g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi))
						{
							char szBuff[255];
							sprintf(szBuff, _("\x4[FeD] \x3%s is now %s"), pi.name, Utils::GetClassByIndex(pEvent->GetInt(XorStr("class").c_str())));
							//sprintf(szBuff, _("\x0x0\x1x1\x2x2\x3x3\x4x4\x5x5\x6x6\x7x7\x8x8\x9x9\x10x10\x11x11\x12x12\x13x13\x14x14\x15x15"));//, pi.name, Utils::GetClassByIndex(pEvent->GetInt(XorStr("class").c_str())));
							//sprintf(szBuff, _("\x1x1\n\x2x2\n\x3x3\n\x4x4\n\x5x5\n\x6x6\n\x7x7\n\x8x8\n\x9x9"), pi.name, Utils::GetClassByIndex(pEvent->GetInt(XorStr("class").c_str())));
							g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nIndex, szBuff);
						}
					/*}*/
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