#pragma once
#include "../../Includes/Includes.h"

class CMoveHelper
{
public:
	void SetHost(CBaseEntity *host) 
	{
		typedef void(__thiscall *FN)(PVOID, CBaseEntity *);
		GetVFunc<FN>(this, 0)(this, host);
	}
};