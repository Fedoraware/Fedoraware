#include "InventoryExpander.h"

CTFInventoryManager* CTFInventoryManager::Get()
{
	using FN = CTFInventoryManager* (__thiscall*)();
	static DWORD dwFn = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 8B 4F 38 8B") + 0x1;
	static DWORD dwEstimate = *reinterpret_cast<PDWORD>(dwFn) + dwFn + 0x4;
	const auto getInventoryManager = reinterpret_cast<FN>(dwEstimate);
	const auto wea = getInventoryManager();
	return wea;
}

CTFPlayerInventory* CTFInventoryManager::GetPlayerInventory()
{
	return reinterpret_cast<CTFPlayerInventory*>(this + 0x108);
}
