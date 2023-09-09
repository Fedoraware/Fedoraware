#include "../Hooks.h"

MAKE_HOOK(C_BasePlayer_CalcViewModelView, S::CBasePlayer_CalcViewModelView(), void, __fastcall,
		  void* ecx, void* edx, CBaseEntity* pOwner, const Vec3& vEyePosition, Vec3& vEyeAngles)
{
	if (I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value) { return Hook.Original<FN>()(ecx, edx, pOwner, vEyePosition, vEyeAngles); }
	if (Vars::Visuals::AimbotViewmodel.Value)
	{
		static Vec3 m_vEyeAngDelayed;
		if (const auto& pLocal = g_EntityCache.GetLocal())
		{
			if (pLocal->IsAlive() && !G::AimPos.IsZero())
			{
				if (G::CurWeaponType == EWeaponType::PROJECTILE)
				{
					vEyeAngles = Math::CalcAngle(vEyePosition, G::PredictedPos);
				}
				else
				{
					vEyeAngles = Math::CalcAngle(vEyePosition, G::AimPos);
				}
				m_vEyeAngDelayed = vEyeAngles;
				G::EyeAngDelay = 0;
			}
			else if (pLocal->IsAlive())
			{
				if (G::EyeAngDelay < 32) { vEyeAngles = m_vEyeAngDelayed; }
				// looks hot ty senator for the idea
				else { vEyeAngles = I::EngineClient->GetViewAngles(); }
			}
		}
	}

	//VM Offsets

	Vec3 vForward = {}, vRight = {}, vUp = {};
	Math::AngleVectors(vEyeAngles, &vForward, &vRight, &vUp);

	// viewmodel offsets should probably be locked between 2 values, just like we should clamp our eye angles, however, I don't want to, f*ck you

	Vec3 vNewEyePosition = vEyePosition + (
		(vRight * Vars::Visuals::VMOffsets.x) +
		(vForward * Vars::Visuals::VMOffsets.y) +
		(vUp * Vars::Visuals::VMOffsets.z)
		);

	vEyeAngles.z += Vars::Visuals::VMRoll.Value; //VM Roll

	Hook.Original<FN>()(ecx, edx, pOwner, vNewEyePosition, vEyeAngles);
}