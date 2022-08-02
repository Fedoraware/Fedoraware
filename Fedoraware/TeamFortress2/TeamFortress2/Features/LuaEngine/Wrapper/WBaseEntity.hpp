#pragma once

class WBaseEntity {
	CBaseEntity* BaseEntity;

public:
	explicit WBaseEntity(CBaseEntity* baseEntity) : BaseEntity(baseEntity) { }

	bool IsValid()
	{
		return BaseEntity != nullptr;
	}

	int GetIndex()
	{
		return BaseEntity->GetIndex();
	}

	Vec3 GetOrigin()
	{
		return BaseEntity->GetAbsOrigin();
	}

	int GetClassID()
	{
		return BaseEntity->GetClientClass()->m_ClassID;
	}

	const char* GetClass()
	{
		return BaseEntity->GetClientClass()->GetName();
	}

	int GetHealth()
	{
		return BaseEntity->GetHealth();
	}

	int GetAmmo()
	{
		return BaseEntity->GetAmmo();
	}

	int GetFlags()
	{
		return BaseEntity->GetFlags();
	}

	Vec3 GetEyePos()
	{
		return BaseEntity->GetEyePosition();
	}

	bool IsDormant()
	{
		return BaseEntity->GetDormant();
	}

	bool IsAlive()
	{
		return BaseEntity->IsAlive();
	}

	int GetTeam()
	{
		return BaseEntity->GetTeamNum();
	}

	void SetOrigin(const Vec3& v)
	{
		BaseEntity->SetAbsOrigin(v);
		BaseEntity->SetVecOrigin(v);
	}

};
