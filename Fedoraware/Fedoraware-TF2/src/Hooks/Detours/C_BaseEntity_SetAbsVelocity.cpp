#include "../Hooks.h"
#include <intrin.h>

namespace S
{
	MAKE_SIGNATURE(C_BasePlayer_PostDataUpdate_SetAbsVelocityCall, CLIENT_DLL, "E8 ? ? ? ? 53 8B CF E8 ? ? ? ? 8D 47 F8 39 05", 0x5);
}

MAKE_HOOK(C_BaseEntity_SetAbsVelocity, S::C_BaseEntity_SetAbsVelocity(), void, __fastcall, void* ecx, void* edx, const Vec3& vecAbsVelocity)
{
	static DWORD dwSetAbsVelocityCall = S::C_BasePlayer_PostDataUpdate_SetAbsVelocityCall();

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == dwSetAbsVelocityCall)
	{
		if (const auto pBasePlayer = static_cast<CBaseEntity*>(ecx))
		{
			if (G::VelFixRecords.contains(pBasePlayer))
			{
				const auto& record = G::VelFixRecords[pBasePlayer];

				const float flSimTimeDelta = pBasePlayer->GetSimulationTime() - record.m_flSimulationTime;
				if (flSimTimeDelta > 0.0f)
				{
					Vec3 vOldOrigin = record.m_vecOrigin;

					const int nCurFlags = pBasePlayer->m_fFlags();
					const int nOldFlags = record.m_nFlags;

					if (!(nCurFlags & FL_ONGROUND) && !(nOldFlags & FL_ONGROUND))
					{
						bool bCorrected = false;

						if ((nCurFlags & FL_DUCKING) && !(nOldFlags & FL_DUCKING))
						{
							vOldOrigin.z += 20.0f;
							bCorrected = true;
						}

						if (!(nCurFlags & FL_DUCKING) && (nOldFlags & FL_DUCKING))
						{
							vOldOrigin.z -= 20.0f;
							bCorrected = true;
						}

						if (bCorrected)
						{
							Vec3 vNewVelocity = vecAbsVelocity;
							vNewVelocity.z = (pBasePlayer->m_vecOrigin().z - vOldOrigin.z) / flSimTimeDelta;
							Hook.Original<FN>()(ecx, edx, vNewVelocity);
							return;
						}
					}
				}
			}
		}
	}

	Hook.Original<FN>()(ecx, edx, vecAbsVelocity);
}
