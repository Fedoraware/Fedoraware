#include "../Hooks.h"
#include "../../Features/Vars.h"

//CClientState::ProcessFixAngle(SVC_FixAngle *)
//CClientState::ProcessCrosshairAngle(SVC_CrosshairAngle *)
//CClientState::ProcessBSPDecal(SVC_BSPDecal *)
//	"ProcessBSPDecal:"
//	@servermsghandler.cpp L669
MAKE_HOOK(ClientState_ProcessFixAngle, g_Pattern.Find(L"engine.dll", L"55 8B EC 8B 45 08 83 EC 08 F3 0F 10 15 ? ? ? ?"), bool, __fastcall,
	void* ecx, void* edx, SVC_FixAngle* msg)
{
	return Vars::Visuals::PreventForcedAngles.Value ? false : Hook.Original<FN>()(ecx, edx, msg);
}