#pragma once
#include "../../Includes/Includes.h"

namespace S
{
	MAKE_SIGNATURE(CTFInventoryManager_GetItemByBackpackPosition, CLIENT_DLL, "55 8B EC 83 EC 08 8B 01", 0x0);
}

class CTFInventoryManager
{
public:
	void* GetItemByBackpackPosition(int iBackpackPosition)
	{
		return S::CTFInventoryManager_GetItemByBackpackPosition.As<void* (__thiscall*)(void*, int)>()(this, iBackpackPosition);
	}
};