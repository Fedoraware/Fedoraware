#include "../Hooks.h"
#include "../../Features/Vars.h"

//CClientState::ProcessFixAngle(SVC_FixAngle *)
//CClientState::ProcessCrosshairAngle(SVC_CrosshairAngle *)
//CClientState::ProcessBSPDecal(SVC_BSPDecal *)
//	"ProcessBSPDecal:"
//	@servermsghandler.cpp L669
MAKE_HOOK(ClientState_ProcessFixAngle, S::ClientState_ProcessFixAngle(), bool, __fastcall, void* ecx, void* edx, SVC_FixAngle* msg)
{
	return Vars::Visuals::PreventForcedAngles.Value ? false : Hook.Original<FN>()(ecx, edx, msg);
}