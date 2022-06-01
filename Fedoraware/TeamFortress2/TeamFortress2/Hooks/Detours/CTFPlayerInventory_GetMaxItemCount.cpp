#include "../Hooks.h"

// This code looks really bad

class CTFPlayerInventory
{
};

class CTFInventoryManager
{
public:
	static CTFInventoryManager* Get();
	CTFPlayerInventory* GetPlayerInventory();
};

CTFInventoryManager* CTFInventoryManager::Get()
{
	using FN = CTFInventoryManager * (__thiscall*)();
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

uintptr_t GetMaxItemCountAddress()
{
	CTFInventoryManager* InventoryManager = nullptr;
	InventoryManager = CTFInventoryManager::Get();

	CTFPlayerInventory* playerinventory = nullptr;
	playerinventory = InventoryManager->GetPlayerInventory();

	return Utils::GetVFuncPtr(playerinventory, 9);
}

MAKE_HOOK(CTFPlayerInventory_GetMaxItemCount, GetMaxItemCountAddress(), int, __fastcall,
		  void* ecx, void* edx)
{
	return 3000;
}