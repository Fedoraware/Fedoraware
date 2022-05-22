#include "../Hooks.h"

#include "../../Features/Resolver/Resolver.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../Features/Menu/Playerlist/Playerlist.h"

MAKE_HOOK(BaseClientDLL_FrameStageNotify, Utils::GetVFuncPtr(I::Client, 35), void, __fastcall,
		  void* ecx, void* edx, EClientFrameStage curStage)
{
	switch (curStage)
	{
		case EClientFrameStage::FRAME_RENDER_START:
		{
			g_GlobalInfo.m_vPunchAngles = Vec3();

			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				// Handle freecam position
				if (g_GlobalInfo.m_bFreecamActive && Vars::Visuals::FreecamKey.m_Var && GetAsyncKeyState(Vars::Visuals::FreecamKey.m_Var) & 0x8000)
				{
					pLocal->SetVecOrigin(g_GlobalInfo.m_vFreecamPos);
					pLocal->SetAbsOrigin(g_GlobalInfo.m_vFreecamPos);
				}

				// Remove punch effect
				if (Vars::Visuals::RemovePunch.m_Var)
				{
					g_GlobalInfo.m_vPunchAngles = pLocal->GetPunchAngles();
					//Store punch angles to be compesnsated for in aim
					pLocal->ClearPunchAngle(); //Clear punch angles for visual no-recoil
				}
			}

			// Resolver
			g_Resolver.Run();

			g_Visuals.SkyboxChanger();

			break;
		}
	}

	Hook.Original<FN>()(ecx, edx, curStage);

	switch (curStage)
	{
		case EClientFrameStage::FRAME_NET_UPDATE_START:
		{
			g_EntityCache.Clear();

			break;
		}


		case EClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		{
			g_AttributeChanger.Run();

			break;
		}


		case EClientFrameStage::FRAME_NET_UPDATE_END:
		{
			g_EntityCache.Fill();
			g_GlobalInfo.m_bLocalSpectated = false;

			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				for (const auto& teammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
				{
					if (teammate->IsAlive() || g_EntityCache.IsFriend(teammate->GetIndex()))
					{
						continue;
					}

					const CBaseEntity* pObservedPlayer = I::EntityList->GetClientEntityFromHandle(teammate->GetObserverTarget());

					if (pObservedPlayer == pLocal)
					{
						g_GlobalInfo.m_bLocalSpectated = true;
						break;
					}
				}
			}

			for (int i = 0; i < I::Engine->GetMaxClients(); i++)
			{
				if (const auto& player = I::EntityList->GetClientEntity(i))
				{
					const VelFixRecord record = { player->m_vecOrigin(), player->m_fFlags(), player->GetSimulationTime() };
					g_GlobalInfo.velFixRecord[player] = record;
				}
			}

			g_PlayerList.UpdatePlayers();
			break;
		}

		case EClientFrameStage::FRAME_RENDER_START:
		{
			if (!g_GlobalInfo.unloadWndProcHook)
			{
				if (Vars::Visuals::Rain.m_Var > 0)
				{
					g_Visuals.rain.Run();
				}

				// genius method i swear
				static bool modded = false;
				if (Vars::Visuals::SkyModulation.m_Var || Vars::Visuals::WorldModulation.m_Var)
				{
					g_Visuals.ModulateWorld();
					modded = true;
				}
				else if (modded)
				{
					modded = false;
					g_Visuals.ModulateWorld();
				}
			}
			break;
		}
	}
}