#include "AutoBlast.h"

#include "../AutoGlobal/AutoGlobal.h"
#include "../../Vars.h"

void CAutoAirblast::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Blast::Active.Value || !G::WeaponCanSecondaryAttack)
	{
		return;
	}

	id = pWeapon->GetWeaponID();

	if (id != TF_WEAPON_FLAMETHROWER && id != TF_WEAPON_FLAME_BALL)
	{
		return;
	}

	if (G::CurItemDefIndex == Pyro_m_ThePhlogistinator)
	{
		return;
	}

	if (Vars::Triggerbot::Blast::DisableOnAttack.Value && pCmd->buttons & IN_ATTACK)
		return;

	if (const auto& pNet = I::EngineClient->GetNetChannelInfo())
	{
		const Vec3 vEyePos = pLocal->GetEyePosition();
		const float flLatency = (pNet->GetLatency(FLOW_INCOMING) + pNet->GetLatency(FLOW_OUTGOING));
		// pretty sure the game shows the predicted position of projectiles so accounting for incoming ping seems useless.
		bool bShouldBlast = false;

		for (const auto& pProjectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
		{
			if (pProjectile->GetTeamNum() == pLocal->GetTeamNum())
			{
				continue; //Ignore team's projectiles
			}

			switch (pProjectile->GetClassID())
			{
				case ETFClassID::CTFGrenadePipebombProjectile:
				case ETFClassID::CTFStunBall:
				{
					if (pProjectile->GetTouched())
					{
						continue; //Ignore landed stickies and sandman balls
					}

					break;
				}

				case ETFClassID::CTFProjectile_Arrow:
				{
					if (pProjectile->GetVelocity().IsZero())
					{
						continue; //Ignore arrows with no velocity / not moving
					}

					break;
				}

				default: break;
			}

			Vec3 vPredicted = (pProjectile->GetAbsOrigin() + pProjectile->GetVelocity().Scale(flLatency / 1000.f));

			//I cant remember if the airblast radius range from 2007 SDK was 185.0f or not..
			/*
				Airblast detection is 256^3
				if the distance between our eyes and the projectile is less than 256 we can airblast this projectile.
				the game doesn't actually fire from eyeposition iirc, it fires from our pelvis (weird).
				I've decided to keep most of this code the same, and just set the "airblast range" to 245, as I don't actually think it matters much.
			*/
			if (vEyePos.DistTo(vPredicted) <= 245.0f && Utils::VisPos(pLocal, pProjectile, vEyePos, vPredicted))
			{
				CGameTrace trace = {};
				static CTraceFilterWorldAndPropsOnly traceFilter = {};
				Utils::TraceHull(pProjectile->GetAbsOrigin(), vPredicted, pProjectile->GetCollideableMaxs(), pProjectile->GetCollideableMaxs() * -1.f, MASK_SHOT_HULL, &traceFilter, &trace);
				if (trace.flFraction < 0.98f && !trace.entity) { continue; }
				if (Vars::Triggerbot::Blast::Rage.Value || Vars::Triggerbot::Blast::Fov.Value == 0)
				{
					pCmd->viewangles = Math::CalcAngle(vEyePos, vPredicted);
					bShouldBlast = true;
					break;
				}
				if (Math::GetFov(I::EngineClient->GetViewAngles(), vEyePos, vPredicted) <= Vars::Triggerbot::Blast::Fov.Value)
				{
					bShouldBlast = true;
					break;
				}
			}
		}

		if (Vars::Triggerbot::Blast::ExtinguishPlayers.Value)
		{
			for (const auto& pBurningPlayer : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
			{
				if (!pBurningPlayer->IsOnFire() || !pBurningPlayer->IsAlive())
					continue;

				if (vEyePos.DistTo(pBurningPlayer->m_vecOrigin()) <= 245.0f && Utils::VisPos(pLocal, pBurningPlayer, vEyePos, pBurningPlayer->m_vecOrigin()))
				{
					if (Vars::Triggerbot::Blast::Rage.Value || Vars::Triggerbot::Blast::Fov.Value == 0)
					{
						pCmd->viewangles = Math::CalcAngle(vEyePos, pBurningPlayer->m_vecOrigin());
						bShouldBlast = true;
						break;
					}
					if (Math::GetFov(I::EngineClient->GetViewAngles(), vEyePos, pBurningPlayer->m_vecOrigin()) <= Vars::Triggerbot::Blast::Fov.Value)
					{
						bShouldBlast = true;
						break;
					}
				}
			}
		}

		if (bShouldBlast)
		{
			if (Vars::Triggerbot::Blast::Rage.Value || Vars::Triggerbot::Blast::Fov.Value == 0 && Vars::Triggerbot::Blast::Silent.Value)
			{
				G::SilentTime = true;
			}

			G::IsAttacking = true;
			pCmd->buttons |= IN_ATTACK2;
		}
	}
}
