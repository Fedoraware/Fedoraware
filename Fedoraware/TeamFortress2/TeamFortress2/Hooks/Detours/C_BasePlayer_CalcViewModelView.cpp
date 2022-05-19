#include "../Hooks.h"

MAKE_HOOK(C_BasePlayer_CalcViewModelView, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 70 8B 55 0C 53 8B 5D 08 89 4D FC 8B 02 89 45 E8 8B 42 04 89 45 EC 8B 42 08 89 45 F0 56 57"), void, __fastcall,
		  void* ecx, void* edx, CBaseEntity* pOwner, const Vec3& vEyePosition, Vec3& vEyeAngles)
{
	static Vec3 m_vEyeAngDelayed;
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->IsAlive() && !g_GlobalInfo.m_vAimPos.IsZero() && Vars::Visuals::AimbotViewmodel.m_Var)
		{
			if (g_GlobalInfo.m_WeaponType == EWeaponType::PROJECTILE)
			{
				vEyeAngles = Math::CalcAngle(vEyePosition, g_GlobalInfo.m_vPredictedPos);
			}
			else
			{
				vEyeAngles = Math::CalcAngle(vEyePosition, g_GlobalInfo.m_vAimPos);
			}
			m_vEyeAngDelayed = vEyeAngles;
			g_GlobalInfo.vEyeAngDelay = 0;
		}
		else if (pLocal->IsAlive())
		{
			if (g_GlobalInfo.vEyeAngDelay < 32) { vEyeAngles = m_vEyeAngDelayed; }
			// looks hot ty senator for the idea
			else { vEyeAngles = I::Engine->GetViewAngles(); }
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

	vEyeAngles.z += Vars::Visuals::VMRoll.m_Var; //VM Roll

	Hook.Original<FN>()(ecx, edx, pOwner, vNewEyePosition, vEyeAngles);
}