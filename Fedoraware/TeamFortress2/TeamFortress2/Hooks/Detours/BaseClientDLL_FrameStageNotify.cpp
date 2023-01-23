#include "../Hooks.h"

#include "../../Features/Resolver/Resolver.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../Features/Menu/Playerlist/Playerlist.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Aimbot/MovementSimulation/MovementSimulation.h"
#include "../../Features/LuaEngine/Callbacks/LuaCallbacks.h"
#include "../../Features/Backtrack/Backtrack.h"

MAKE_HOOK(BaseClientDLL_FrameStageNotify, Utils::GetVFuncPtr(I::BaseClientDLL, 35), void, __fastcall,
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
				{
					G::PunchAngles = pLocal->GetPunchAngles();	//	use in aimbot 
					if (Vars::Visuals::RemovePunch.Value) { pLocal->ClearPunchAngle(); }	//	visual no-recoil
				}
			}

			// Resolver
			F::Resolver.FrameStageNotify();

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
			F::Backtrack.FrameStageNotify();
			F::MoveSim.FillVelocities();
			F::Visuals.FillSightlines();
			G::BulletTracerFix = true;
			G::LocalSpectated = false;
			F::Visuals.PruneBulletTracers();
			if (const auto& pLocal = g_EntityCache.GetLocal())
			{
				for (const auto& teammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
				{
					if (teammate->IsAlive() || g_EntityCache.IsFriend(teammate->GetIndex()))
					{
						continue;
					}

					const CBaseEntity* pObservedPlayer = I::ClientEntityList->GetClientEntityFromHandle(teammate->GetObserverTarget());

					if (pObservedPlayer == pLocal)
					{
						G::LocalSpectated = true;
						break;
					}
				}
			}

			for (int i = 0; i < I::EngineClient->GetMaxClients(); i++)
			{
				if (const auto& player = I::ClientEntityList->GetClientEntity(i))
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

				if (Vars::Visuals::SkyModulation.Value || Vars::Visuals::WorldModulation.Value)
				{
					F::Visuals.ModulateWorld();
				}
			}
			break;
		}
	}

	F::LuaCallbacks.OnFrameStage(static_cast<int>(curStage));
}