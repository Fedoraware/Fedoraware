#pragma once
#include "../../Includes/Includes.h"

class CTFInventoryManager
{
public:
	void* GetItemByBackpackPosition(int iBackpackPosition)
	{
		return reinterpret_cast<void* (__thiscall*)(void*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 8B 01"))(this, iBackpackPosition);
	}
};