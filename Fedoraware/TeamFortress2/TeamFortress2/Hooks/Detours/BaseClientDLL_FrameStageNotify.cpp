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
			G::PunchAngles = Vec3();

			if (const auto& pLocal = g_EntityCache.GetLocal())
			{
				// Handle freecam position
				if (G::FreecamActive && Vars::Visuals::FreecamKey.Value && GetAsyncKeyState(Vars::Visuals::FreecamKey.Value) & 0x8000)
				{
					pLocal->SetVecOrigin(G::FreecamPos);
					pLocal->SetAbsOrigin(G::FreecamPos);
				}

				// Remove punch effect
				if (Vars::Visuals::RemovePunch.Value)
				{
					G::PunchAngles = pLocal->GetPunchAngles();
					//Store punch angles to be compesnsated for in aim
					pLocal->ClearPunchAngle(); //Clear punch angles for visual no-recoil
				}
			}

			// Resolver
			F::Resolver.Run();

			F::Visuals.SkyboxChanger();

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
			F::AttributeChanger.Run();

			break;
		}


		case EClientFrameStage::FRAME_NET_UPDATE_END:
		{
			g_EntityCache.Fill();
			G::LocalSpectated = false;

			if (const auto& pLocal = g_EntityCache.GetLocal())
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
						G::LocalSpectated = true;
						break;
					}
				}
			}

			for (int i = 0; i < I::Engine->GetMaxClients(); i++)
			{
				if (const auto& player = I::EntityList->GetClientEntity(i))
				{
					const VelFixRecord record = { player->m_vecOrigin(), player->m_fFlags(), player->GetSimulationTime() };
					G::VelFixRecords[player] = record;
				}
			}

			F::PlayerList.UpdatePlayers();
			break;
		}

		case EClientFrameStage::FRAME_RENDER_START:
		{
			if (!G::UnloadWndProcHook)
			{
				if (G::ShouldUpdateMaterialCache)
				{
					F::Visuals.ClearMaterialHandles();
					F::Visuals.StoreMaterialHandles();
					G::ShouldUpdateMaterialCache = false;
				}
				if (Vars::Visuals::Rain.Value > 0)
				{
					F::Visuals.rain.Run();
				}

				// genius method i swear
				static bool modded = false;
				if (Vars::Visuals::SkyModulation.Value || Vars::Visuals::WorldModulation.Value)
				{
					F::Visuals.ModulateWorld();
					modded = true;
				}
				else if (modded)
				{
					modded = false;
					F::Visuals.ModulateWorld();
				}
			}
			break;
		}
	}
}