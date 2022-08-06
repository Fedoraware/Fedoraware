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
		if (!IsValid()) { return 0; }
		return BaseEntity->GetIndex();
	}

	Vec3 GetOrigin()
	{
		if (!IsValid()) { return { }; }
		return BaseEntity->GetAbsOrigin();
	}

	int GetClassID()
	{
		if (!IsValid()) { return 0; }
		return BaseEntity->GetClientClass()->m_ClassID;
	}

	const char* GetClass()
	{
		if (!IsValid()) { return "Invalid"; }
		return BaseEntity->GetClientClass()->GetName();
	}

	int GetHealth()
	{
		if (!IsValid()) { return 0; }
		return BaseEntity->GetHealth();
	}

	int GetAmmo()
	{
		if (!IsValid()) { return 0; }
		return BaseEntity->GetAmmo();
	}

	int GetFlags()
	{
		if (!IsValid()) { return 0; }
		return BaseEntity->GetFlags();
	}

	Vec3 GetEyePos()
	{
		if (!IsValid()) { return { }; }
		return BaseEntity->GetEyePosition();
	}

	bool IsDormant()
	{
		if (!IsValid()) { return false; }
		return BaseEntity->GetDormant();
	}

	bool IsAlive()
	{
		if (!IsValid()) { return false; }
		return BaseEntity->IsAlive();
	}

	int GetTeam()
	{
		if (!IsValid()) { return 0; }
		return BaseEntity->GetTeamNum();
	}

	void SetOrigin(const Vec3& v)
	{
		if (!IsValid()) { return; }
		BaseEntity->SetAbsOrigin(v);
		BaseEntity->SetVecOrigin(v);
	}

};
