#pragma once

class WBaseCombatWeapon {
	CBaseCombatWeapon* Weapon = nullptr;

public:
	explicit WBaseCombatWeapon(CBaseCombatWeapon* weapon) : Weapon(weapon) { }

	bool IsValid()
	{
		return Weapon != nullptr;
	}

	WBaseEntity GetEntity()
	{
		if (!IsValid()) { return WBaseEntity(nullptr); }
		return WBaseEntity(Weapon->GetGroundEntity());
	}

	const char* GetName()
	{
		if (!IsValid()) { return "Invalid"; }
		return Weapon->GetName();
	}

	bool CanShoot()
	{
		const auto& pLocal = g_EntityCache.GetLocal();
		if (IsValid() && pLocal)
		{
			return Weapon->CanShoot(pLocal);
		}

		return false;
	}

	int GetClip1()
	{
		if (!IsValid()) { return 0; }
		return Weapon->GetClip1();
	}

	int GetClip2()
	{
		if (!IsValid()) { return 0; }
		return Weapon->GetClip2();
	}

	int GetSlot()
	{
		if (!IsValid()) { return 0; }
		return Weapon->GetSlot();
	}

	int GetWeaponID()
	{
		if (!IsValid()) { return 0; }
		return Weapon->GetWeaponID();
	}

	bool IsInReload()
	{
		if (!IsValid()) { return false; }
		return Weapon->IsInReload();
	}

	float GetDamage()
	{
		if (!IsValid()) { return 0; }
		return (float)*(int*)((*(DWORD*)(Weapon + 0xB24) << 6) + *(DWORD*)(Weapon + 0xB30) + 0x6F8);
		//return Weapon->GetWeaponData().m_nDamage;
	}

	float GetBulletsPerShot()
	{
		if (!IsValid()) { return 0; }
		return (float)*(int*)((*(DWORD*)(Weapon + 0xB24) << 6) + *(DWORD*)(Weapon + 0xB30) + 0x6FC);
		//return Weapon->GetWeaponData().m_nBulletsPerShot;
	}

};