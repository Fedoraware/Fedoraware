#include "AutoDetonate.h"

#include "../../Vars.h"

//credits to KGB
class CEntitySphereQuery
{
public:
	CEntitySphereQuery(const Vec3& center, const float radius, const int flagMask = 0, const int partitionMask = PARTITION_CLIENT_NON_STATIC_EDICTS) {
		static DWORD dwAddress = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 14 D9 45 0C"));
		reinterpret_cast<void(__thiscall*)(void*, const Vec3&, float, int, int)>(dwAddress)(this, center, radius, flagMask, partitionMask);
	}

	CBaseEntity* GetCurrentEntity() {
		return (m_nListIndex < m_nListCount) ? m_pList[m_nListIndex] : nullptr;
	}

	void NextEntity() {
		m_nListIndex++;
	}

private:
	int m_nListIndex, m_nListCount;
	CBaseEntity* m_pList[MAX_SPHERE_QUERY];
};

void CAutoDetonate::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Detonate::Active.m_Var)
		return;

	m_bDetonated = false;

	if (Vars::Triggerbot::Detonate::Stickies.m_Var)
	{
		m_flRadius = (115.0f * Vars::Triggerbot::Detonate::RadiusScale.m_Var);

		for (const auto& Sticky : g_EntityCache.GetGroup(EGroupType::LOCAL_STICKIES))
		{
			CBaseEntity* pEntity = nullptr;

			for (CEntitySphereQuery Sphere(Sticky->GetWorldSpaceCenter(), m_flRadius); (pEntity = Sphere.GetCurrentEntity()) != nullptr; Sphere.NextEntity())
			{
				if (!pEntity || pEntity == pLocal || !pEntity->IsAlive() || pEntity->GetTeamNum() == pLocal->GetTeamNum())
					continue;

				const bool bIsPlayer = pEntity->IsPlayer();
				if (bIsPlayer || pEntity->IsBuilding())
				{
					if (bIsPlayer)
					{
						if (Vars::Triggerbot::Global::IgnoreFriends.m_Var && g_EntityCache.Friends[pEntity->GetIndex()])
							continue;

						if (Vars::Triggerbot::Global::IgnoreCloaked.m_Var && pEntity->IsCloaked())
							continue;

						if (Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var && !pEntity->IsVulnerable())
							continue;
					}

					CTraceFilterWorldAndPropsOnly Filter = { };
					CGameTrace Trace = { };
					Utils::Trace(Sticky->GetWorldSpaceCenter(), pEntity->GetWorldSpaceCenter(), MASK_SOLID, &Filter, &Trace);

					if (Trace.flFraction >= 0.99f || Trace.entity == pEntity) {
						m_bDetonated = true;
						break;
					}
				}
			}

			if (m_bDetonated)
				break;
		}
	}

	if (Vars::Triggerbot::Detonate::Flares.m_Var)
	{
		m_flRadius = 85.0f * Vars::Triggerbot::Detonate::RadiusScale.m_Var;

		//There should only be one in existance at the time
		//Old flare will blow up / vanish before the slow shit reloads
		for (const auto& pFlare : g_EntityCache.GetGroup(EGroupType::LOCAL_FLARES))
		{
			CBaseEntity* pEntity = nullptr;

			for (CEntitySphereQuery Sphere(pFlare->GetWorldSpaceCenter(), m_flRadius); (pEntity = Sphere.GetCurrentEntity()) != nullptr; Sphere.NextEntity())
			{
				if (!pEntity || pEntity == pLocal || !pEntity->IsAlive() || pEntity->GetTeamNum() == pLocal->GetTeamNum())
					continue;

				const bool bIsPlayer = pEntity->IsPlayer();
				if (bIsPlayer || pEntity->IsBuilding())
				{
					if (bIsPlayer)
					{
						if (Vars::Triggerbot::Global::IgnoreFriends.m_Var && g_EntityCache.Friends[pEntity->GetIndex()])
							continue;

						if (Vars::Triggerbot::Global::IgnoreCloaked.m_Var && pEntity->IsCloaked())
							continue;

						if (Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var && !pEntity->IsVulnerable())
							continue;
					}

					CTraceFilterWorldAndPropsOnly Filter = { };
					CGameTrace Trace = { };
					Utils::Trace(pFlare->GetWorldSpaceCenter(), pEntity->GetWorldSpaceCenter(), MASK_SOLID, &Filter, &Trace);

					if (Trace.flFraction >= 0.99f || (Trace.entity == pEntity && Trace.entity != pLocal)) {
						m_bDetonated = true;
						break;
					}
				}
			}

			if (m_bDetonated)
				break;
		}
	}

	if (m_bDetonated)
		pCmd->buttons |= IN_ATTACK2;
}