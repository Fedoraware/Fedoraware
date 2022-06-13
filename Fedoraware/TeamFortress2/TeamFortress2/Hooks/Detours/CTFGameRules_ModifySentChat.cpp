#include "../Hooks.h"

struct s_CTFGameRules
{
	char pad[1031];
	bool m_bPlayingMedieval;
};

MAKE_HOOK(CTFGameRules_ModifySentChat, g_Pattern.Find(L"client.dll", L"55 8B EC 80 B9 ? ? ? ? ? 56 8B 75 08 74 26"), void, __fastcall,
		  void* ecx, void* edx, char* pBuf, int iBufSize)
{
	if (Vars::Misc::ForceMedievalChat.Value)
	{
		if (auto pGameRules = reinterpret_cast<s_CTFGameRules*>(ecx))
		{
			ConVar* tf_medieval_autorp = I::CVars->FindVar("tf_medieval_autorp");
			ConVar* english = I::CVars->FindVar("english");
			if (tf_medieval_autorp && english)
			{
				bool bOriginalAutoRP = tf_medieval_autorp->GetBool();
				bool bOriginalEnglish = english->GetBool();
				bool bOriginalPlayingMedieval = pGameRules->m_bPlayingMedieval;
				pGameRules->m_bPlayingMedieval = true;
				tf_medieval_autorp->SetValue(true);
				english->SetValue(true);
				Hook.Original<FN>()(ecx, edx, pBuf, iBufSize);
				pGameRules->m_bPlayingMedieval = bOriginalPlayingMedieval;
				tf_medieval_autorp->SetValue(bOriginalAutoRP);
				english->SetValue(bOriginalEnglish);
				return;
			}
		}
	}

	Hook.Original<FN>()(ecx, edx, pBuf, iBufSize);
}