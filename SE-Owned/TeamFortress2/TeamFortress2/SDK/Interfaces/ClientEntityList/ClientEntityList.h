#pragma once
#include "../../Includes/Includes.h"

class CClientEntityList
{
public:
	CBaseEntity *GetClientEntity(int nEntityIndex)
	{
		typedef CBaseEntity *(__thiscall *FN)(PVOID, int);
		return GetVFunc<FN>(this, 3)(this, nEntityIndex);
	}

	CBaseEntity *GetClientEntityFromHandle(int hEntity)
	{
		typedef CBaseEntity *(__thiscall *FN)(PVOID, int);
		return GetVFunc<FN>(this, 4)(this, hEntity);
	}

	int GetHighestEntityIndex()
	{
		typedef int(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 6)(this);
	}
};

#define VCLIENTENTITYLIST_INTERFACE_VERSION	"VClientEntityList003"