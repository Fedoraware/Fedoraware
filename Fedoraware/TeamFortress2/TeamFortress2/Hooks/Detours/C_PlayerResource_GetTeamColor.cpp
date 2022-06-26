#include "../Hooks.h"

//MAKE_HOOK(C_PlayerResource, g_Pattern.Find(L"client.dll", L"55 8B EC 8B 45 08 83 F8 1F"), Color_t&, __fastcall,
//		  void* ecx, void* edx, int index)
//{
//	static std::map<void*, bool> returns;
//
//	auto retaddr = _ReturnAddress();
//	if (returns.find(retaddr) == returns.end())
//	{
//		returns[retaddr] = true;
//		I::CVars->ConsolePrintf("%p\n", retaddr);
//	}
//	
//
//	I::CVars->ConsolePrintf("%d", index);
//
//	static auto CTFClientScoreBoardDialog_UpdatePlayerList_ReturnAddress_g_PR = g_Pattern.Find(L"client.dll", L"80 7D F8 ? 8B 08");
//	static auto CTFClientScoreBoardDialog_UpdatePlayerList_ReturnAddress_Other = g_Pattern.Find(L"client.dll", L"8B 8B ? ? ? ? 8B 30 8B 01 56");
//
//	DWORD ReturnAddress = reinterpret_cast<DWORD>(_ReturnAddress());
//
//	if (ReturnAddress == CTFClientScoreBoardDialog_UpdatePlayerList_ReturnAddress_g_PR ||
//		ReturnAddress == CTFClientScoreBoardDialog_UpdatePlayerList_ReturnAddress_Other)
//	{
//		CBaseEntity* pEntity = I::EntityList->GetClientEntity(index);
//		if (pEntity)
//		{
//			Color_t ret = Utils::Rainbow();//Utils::GetEntityDrawColor(pEntity, Vars::ESP::Main::EnableTeamEnemyColors.Value);
//			return ret;
//		}	
//	}
//
//	return Hook.Original<FN>()(ecx, edx, index);
//}