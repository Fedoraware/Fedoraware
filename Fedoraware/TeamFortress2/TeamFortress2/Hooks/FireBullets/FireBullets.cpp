#include "FireBullets.h"


void __fastcall FireBullets::Hook(void* ecx, void* edx, CBaseCombatWeapon* pWeapon, const FireBulletsInfo_t& info,
                                  bool bDoEffects, int nDamageType, int nCustomDamageType)
{
	static auto original = Func.Original<fn>();

	if (!pWeapon || (!Vars::Visuals::ParticleTracer.m_Var && !Vars::Visuals::BulletTracer.m_Var))
	{
		return original(ecx, edx, pWeapon, info, bDoEffects, nDamageType, nCustomDamageType);
	}
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		Vec3 vStart = info.m_vecSrc;
		Vec3 vEnd = vStart + info.m_vecDirShooting * info.m_flDistance;

		CGameTrace trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pLocal;

		/* if ur shooting thru stuff, change MASK_SHOT to MASK_SOLID - myzarfin */
		Utils::Trace(vStart, vEnd, (MASK_SHOT /* | CONTENTS_GRATE | MASK_VISIBLE*/), &filter, &trace);
		//g_Interfaces.EngineTrace->TraceRay(Ray, (MASK_SOLID | CONTENTS_HITBOX), NULL, &trace);
		int iAttachment = pWeapon->LookupAttachment(_("muzzle"));
		pWeapon->GetAttachment(iAttachment, trace.vStartPos);
		
		if (Vars::Visuals::BulletTracer.m_Var)
		{
			Color_t Color = Vars::Visuals::BulletTracerRainbow.m_Var ? Utils::Rainbow() : Colors::BulletTracer;

			g_Interfaces.DebugOverlay->AddLineOverlayAlpha(trace.vStartPos, trace.vEndPos, Color.r, Color.g, Color.b,
			                                               Colors::BulletTracer.a, true, 5);
		}
		if (!pLocal->IsInValidTeam())
		{
			return;
		}

		int team = pLocal->GetTeamNum();

		switch (Vars::Visuals::ParticleTracer.m_Var)
		{
		//Machina
		case 1:
			Particles::ParticleTracer(team == 2 ? "dxhr_sniper_rail_red" : "dxhr_sniper_rail_blue", trace.vStartPos, trace.vEndPos,
			               pLocal->GetIndex(), iAttachment, true);
			break;

		//C.A.P.P.E.R
		case 2:
			pLocal->IsCritBoosted()
				? (Particles::ParticleTracer(team == 2 ? "bullet_tracer_raygun_red_crit" : "bullet_tracer_raygun_blue_crit",
				                  trace.vStartPos, trace.vEndPos, pLocal->GetIndex(), iAttachment, true))
				: (Particles::ParticleTracer(team == 2 ? "bullet_tracer_raygun_red" : "bullet_tracer_raygun_blue", trace.vStartPos,
				                  trace.vEndPos, pLocal->GetIndex(), iAttachment, true));
			break;

		//Short circuit
		case 3:
			Particles::ParticleTracer(team == 2 ? "dxhr_lightningball_hit_zap_red" : "dxhr_lightningball_hit_zap_blue",
			               trace.vStartPos, trace.vEndPos, pLocal->GetIndex(), iAttachment, true);
			break;

		//Merasmus ZAP
		case 4:
			Particles::ParticleTracer("merasmus_zap", trace.vStartPos, trace.vEndPos, pLocal->GetIndex(), iAttachment, true);
			break;

		//Merasmus ZAP Beam 2
		case 5:
			Particles::ParticleTracer("merasmus_zap_beam02", trace.vStartPos, trace.vEndPos, pLocal->GetIndex(), iAttachment,
			               true);
			break;

		case 6:
			pLocal->IsCritBoosted()
				? (Particles::ParticleTracer(
					team == 2 ? "bullet_bignasty_tracer01_red_crit" : "bullet_bignasty_tracer01_blue_crit",
					trace.vStartPos, trace.vEndPos, pLocal->GetIndex(), iAttachment, true))
				: (Particles::ParticleTracer(team == 2 ? "bullet_bignasty_tracer01_blue" : "bullet_bignasty_tracer01_red",
				                  trace.vStartPos, trace.vEndPos, pLocal->GetIndex(), iAttachment, true));
			break;

		case 7:
			Particles::ParticleTracer("tfc_sniper_distortion_trail", trace.vStartPos, trace.vEndPos, pLocal->GetIndex(),
			               iAttachment, true);
			break;

		case 8: // black_ink, demo'd: https://youtu.be/Ba0lcMOfm9w 
			Particles::ParticleTracer("merasmus_zap_beam01", trace.vStartPos, trace.vEndPos, pLocal->GetIndex(), iAttachment,
			               true);
			break;

		case 9:
			// custom particle tracer, def not pasted from deathpole or anything. list @ dump_particlemanifest or @ https://github.com/tf2cheater2013/particles.txt
			Particles::ParticleTracer(Vars::Visuals::ParticleName.c_str(), trace.vStartPos, trace.vEndPos, pLocal->GetIndex(),
			               iAttachment, true);
			break;
		}
	}
}

void FireBullets::Init()
{
	static auto dwFireBullets = g_Pattern.Find(
		_(L"client.dll"),
		_(L"53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C ? ? 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 4B"));
	Func.Hook(reinterpret_cast<void*>(dwFireBullets), reinterpret_cast<void*>(Hook));
}
