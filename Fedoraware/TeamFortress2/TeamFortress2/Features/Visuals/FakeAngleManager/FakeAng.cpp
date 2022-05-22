#include "FakeAng.h"

void CFakeAng::Run(CUserCmd* pCmd) {
	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		if (!pCmd) {
			return;
		}
		if (pLocal->IsAlive()) {

			if (const auto& pAnimState = pLocal->GetAnimState()) {

				matrix3x4 bones[128];
				if (pLocal->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, I::GlobalVars->curtime)) {
					BoneMatrix = *reinterpret_cast<FakeMatrixes*>(bones);
				}
			}
		}

	}
}