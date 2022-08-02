#pragma once

class WBaseEntity {
	CBaseEntity* BaseEntity;

public:
	explicit WBaseEntity(CBaseEntity* baseEntity) : BaseEntity(baseEntity) { }

	void AddCond(int cond) { BaseEntity->AddCond(cond); }
	int GetAmmo() { return BaseEntity->GetAmmo(); }
	Vec3 GetAbsOrigin() { return BaseEntity->GetAbsOrigin(); }
	int GetHealth() { return BaseEntity->GetHealth(); }
	bool IsAlive() { return BaseEntity->IsAlive(); }
};