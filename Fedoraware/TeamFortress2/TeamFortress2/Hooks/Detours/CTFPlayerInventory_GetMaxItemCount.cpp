#include "../Hooks.h"

class CTFPlayerInventory;
class CTFInventoryManager;

DWORD GetMaxItemCountAddress( ) {
	static auto GetInventoryManager = reinterpret_cast< CTFInventoryManager * ( __thiscall* )( ) >( 
		g_Pattern.Find( L"client.dll", L"B8 20 D5 C7 10 C3" ) );

	auto inventory_manager = GetInventoryManager();
	if ( inventory_manager == nullptr ) {
		return NULL;
	}

	auto player_inventory = reinterpret_cast< CTFInventoryManager* >( reinterpret_cast<DWORD>(inventory_manager) + 0x108 );
	if ( player_inventory == nullptr ) {
		return NULL;
	}

	return Utils::GetVFuncPtr( player_inventory, 9 );
}

MAKE_HOOK( CTFPlayerInventory_GetMaxItemCount, GetMaxItemCountAddress( ), int, __fastcall,
	void* ecx, void* edx ) {
	return 3000;
}