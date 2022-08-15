#include "../Hooks.h"

struct s_CTFGameRules
{
	char pad[1031];
	bool m_bPlayingMedieval;
};

MAKE_HOOK(CTFGameRules_ModifySentChat, g_Pattern.Find(L"client.dll", L"55 8B EC 80 B9 ? ? ? ? ? 56 8B 75 08 74 26"), void, __fastcall,
		  void* ecx, void* edx, char* pBuf, int iBufSize)
{
	if (Vars::Misc::MedievalChat.Value > 0)
	{
		if(const auto pGameRules = static_cast<s_CTFGameRules*>(ecx))
		{
			ConVar* tf_medieval_autorp = I::Cvar->FindVar("tf_medieval_autorp");
			ConVar* english = I::Cvar->FindVar("english");
			if (tf_medieval_autorp && english)
			{
				const bool bOriginalAutoRP = tf_medieval_autorp->GetBool();
				const bool bOriginalEnglish = english->GetBool();
				const bool bOriginalPlayingMedieval = pGameRules->m_bPlayingMedieval;
				const bool forceMedieval = Vars::Misc::MedievalChat.Value == 2;

				pGameRules->m_bPlayingMedieval = forceMedieval;
				tf_medieval_autorp->SetValue(forceMedieval);
				english->SetValue(forceMedieval);

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