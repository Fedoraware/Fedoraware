#include "../Hooks.h"

//static Color_t nonLocalRedDead = { 135, 83, 83, 255 };
//static Color_t localRedDead = { 182, 75, 75, 255 };
//static Color_t nonLocalBluDead = { 81, 97, 129, 255 };
//static Color_t localBluDead = { 123, 153, 187, 255 };
//
//bool AreColorsEqual(Color_t& a, Color_t& b)
//{
//	if (a.r == b.r &&
//		a.g == b.g &&
//		a.b == b.b &&
//		a.a == b.a)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//
//
//MAKE_HOOK(SectionedListPanel_SetItemFgColor, g_Pattern.Find(L"client.dll", L"55 8B EC 8B 55 08 56 8B F1 3B 96 ? ? ? ? 73 55"), void, __fastcall,
//		  void* ecx, void* edx, int itemID, Color_t color)
//{
//	static auto CTFClientScoreBoardDialog_UpdatePlayerList_ReturnAddress = g_Pattern.Find(L"client.dll", L"8B 4D ? C7 45 C4 ? ? ? ? FF 75");
//
//	DWORD returnAddress = reinterpret_cast<DWORD>(_ReturnAddress());
//
//	//if (returnAddress == CTFClientScoreBoardDialog_UpdatePlayerList_ReturnAddress)
//	//{
//	//	Hook.Original<FN>()(ecx, edx, itemID, Utils::Rainbow(itemID));
//	//}
//	//else
//	//{
//		Hook.Original<FN>()(ecx, edx, itemID, color);
//	//}
//	//auto retaddr = _ReturnAddress();
//	//if (TEST::MapOfReturnAddresses.find(retaddr) == TEST::MapOfReturnAddresses.end())
//	//{
//	//	TEST::MapOfReturnAddresses[_ReturnAddress()] = true;
//	//	printf("%p\n", retaddr);
//	//}
//	//Hook.Original<FN>()(ecx, edx, itemID, Utils::Rainbow());
//}