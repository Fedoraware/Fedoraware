#include "FakeAng.h"

//gets called after fakelag.

void CFakeAng::Run(CUserCmd* pCmd)
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (!pCmd)
		{
			return;
		}
		if (pLocal->IsAlive())
		{

			if (const auto& pAnimState = pLocal->GetAnimState())
			{
				Math::Clamp(G::FakeViewAngles.x, -89.f, 89.f);

				float flOldFrameTime = I::GlobalVars->frametime;
				int nOldSequence = pLocal->m_nSequence();
				float flOldCycle = pLocal->m_flCycle();
				auto pOldPoseParams = pLocal->GetPoseParam();
				char pOldAnimState[sizeof(CMultiPlayerAnimState)] = {};

				memcpy(pOldAnimState, pAnimState, sizeof(CMultiPlayerAnimState));

				auto Restore = [&]()
				{
					I::GlobalVars->frametime = flOldFrameTime;
					pLocal->m_nSequence() = nOldSequence;
					pLocal->m_flCycle() = flOldCycle;
					pLocal->SetPoseParam(pOldPoseParams);
					memcpy(pAnimState, pOldAnimState, sizeof(CMultiPlayerAnimState));
				};

				I::GlobalVars->frametime = 0.0f;

				pAnimState->m_flCurrentFeetYaw = G::FakeViewAngles.y;
				pAnimState->m_flGoalFeetYaw = G::FakeViewAngles.y;
				pAnimState->Update(G::FakeViewAngles.y, G::FakeViewAngles.x);

				matrix3x4 bones[128];
				if (pLocal->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, I::GlobalVars->curtime))
				{
					BoneMatrix = *reinterpret_cast<FakeMatrixes*>(bones);
				}

				Restore();
			}
		}

	}
}