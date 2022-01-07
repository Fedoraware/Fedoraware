#pragma once
#include "../../SDK/SDK.h"

class CTFPlayerInventory {
};

class CTFInventoryManager {
public:
	static CTFInventoryManager* Get();
	CTFPlayerInventory* GetPlayerInventory();
};

namespace InventoryExpander {
	inline SEOHook::Func Func;

	inline int __fastcall Expand(void* ecx, void* edx) {
		return 3000;
	}
}