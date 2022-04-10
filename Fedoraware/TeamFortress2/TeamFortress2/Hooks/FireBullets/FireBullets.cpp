#include "FireBullets.h"

void DrawBeam(Vector source, Vector end)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = 0;
	beamInfo.m_pszModelName = Vars::Visuals::Beans::UseCustomModel.m_Var ? Vars::Visuals::Beans::Model.c_str() : "sprites/physbeam.vmt";
	beamInfo.m_nModelIndex = -1; // will be set by CreateBeamPoints if its -1
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = Vars::Visuals::Beans::Life.m_Var;
	beamInfo.m_flWidth = Vars::Visuals::Beans::Width.m_Var;
	beamInfo.m_flEndWidth = Vars::Visuals::Beans::EndWidth.m_Var;
	beamInfo.m_flFadeLength = Vars::Visuals::Beans::FadeLength.m_Var;
	beamInfo.m_flAmplitude = Vars::Visuals::Beans::Amplitude.m_Var;
	beamInfo.m_flBrightness = Vars::Visuals::Beans::Brightness.m_Var;
	beamInfo.m_flSpeed = Vars::Visuals::Beans::Speed.m_Var;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0;
	beamInfo.m_flRed = Vars::Visuals::Beans::Rainbow.m_Var ? (static_cast<float>(floor(sin(g_Interfaces.GlobalVars->curtime + 0) * 128.0f + 128.0f))) : Vars::Visuals::Beans::BeamColour.r;
	beamInfo.m_flGreen = Vars::Visuals::Beans::Rainbow.m_Var ? (static_cast<float>(floor(sin(g_Interfaces.GlobalVars->curtime + 2) * 128.0f + 128.0f))) : Vars::Visuals::Beans::BeamColour.g;
	beamInfo.m_flBlue = Vars::Visuals::Beans::Rainbow.m_Var ? (static_cast<float>(floor(sin(g_Interfaces.GlobalVars->curtime + 4) * 128.0f + 128.0f))) : Vars::Visuals::Beans::BeamColour.b;
	beamInfo.m_nSegments = Vars::Visuals::Beans::segments.m_Var;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = Vars::Visuals::Beans::Flags.m_Var;
	beamInfo.m_vecStart = source;
	beamInfo.m_vecEnd = end;
	Beam_t* coolBeam = g_Interfaces.ILOVEBEAMS->CreateBeamPoints(beamInfo);
	if (coolBeam)
	{
		g_Interfaces.ILOVEBEAMS->DrawBeam(coolBeam);
	}
}

void __fastcall FireBullets::Hook(void* ecx, void* edx, CBaseCombatWeapon* pWeapon, const FireBulletsInfo_t& info,
                                  bool bDoEffects, int nDamageType, int nCustomDamageType)
{
	static auto original = Func.Original<fn>();

	if (!pWeapon || (!Vars::Visuals::ParticleTracer.m_Var && !Vars::Visuals::BulletTracer.m_Var && !Vars::Visuals::Beans::Active.m_Var))
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

		/*
				namespace Beans
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> Rainbow { false, L"Rainbow" };
			inline Color_t BeamColour{ 255, 255, 255, 255 };
			inline CVar<bool> UseCustomModel { false, L"Use Custom Model" };
			inline CVar<std::string> Model{ "sprites/physbeam.vmt", L"Model" };
			inline CVar<float> Life { 2.f, L"Life" };
			inline CVar<float> Width { 2.f, L"Width" };
			inline CVar<float> EndWidth { 2.f, L"End Width" };
			inline CVar<float> FadeLength { 10.f, L"Fade Length" };
			inline CVar<float> Amplitude { 2.f, L"Amplitude" };
			inline CVar<float> Brightness { 255.f, L"Brightness" };
			inline CVar<float> Speed { 0.2f, L"Speed" };
			inline CVar<int> Flags { 65792, L"Flags" }; // FBEAM_ONLYNOISEONCE | FBEAM_REVERSED
		}*/

		if (Vars::Visuals::Beans::Active.m_Var)
		{
			DrawBeam(trace.vStartPos, trace.vEndPos);
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
