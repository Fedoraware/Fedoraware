#include "../Hooks.h"

//CClientState::ProcessFixAngle(SVC_FixAngle *)
//CClientState::ProcessCrosshairAngle(SVC_CrosshairAngle *)
//CClientState::ProcessBSPDecal(SVC_BSPDecal *)
//	"ProcessBSPDecal:"
//	@servermsghandler.cpp L669
MAKE_HOOK(ClientState_ProcessFixAngle, g_Pattern.Find(L"engine.dll", L"55 8B EC 8B 45 08 83 EC 08 F3 0F 10 15 ? ? ? ?"), bool, __fastcall,
	void* CNetChan, void* edi, SVC_FixAngle* msg)
{
	return false;
	//msg->m_bRelative = true;
	//msg->m_Angle = Vec3(0.f, 0.f, 0.f);
	//return Hook.Original<FN>()(CNetChan, edi, msg);
}