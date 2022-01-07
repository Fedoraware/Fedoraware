#include "InventoryExpander.h"

CTFInventoryManager* CTFInventoryManager::Get() {
	typedef CTFInventoryManager* (__thiscall* FN)();
	static DWORD dwFn = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 8B 4F 38 8B") + 0x1;
	static DWORD dwEstimate = *(PDWORD)dwFn + dwFn + 0x4;
	FN getInventoryManager = (FN)dwEstimate;
	auto wea = getInventoryManager();
	return wea;
}

CTFPlayerInventory* CTFInventoryManager::GetPlayerInventory() {
	return (CTFPlayerInventory*)(this + 0x108);
}
