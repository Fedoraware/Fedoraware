#include "../Hooks.h"

#define TF_VISION_FILTER_NONE			0
#define TF_VISION_FILTER_PYRO			(1<<0)		// 1
#define TF_VISION_FILTER_HALLOWEEN		(1<<1)		// 2
#define TF_VISION_FILTER_ROME			(1<<2)		// 4

MAKE_HOOK(IsLocalPlayerUsingVisionFilterFlags, g_Pattern.Find(L"client.dll", L"55 8B EC 8A 45 ? 56 8B 35"), bool, __cdecl,
		  int nFlags, bool bWeaponsCheck)
{
	switch (Vars::Visuals::Vision.m_Var)
	{
		case 1:
		{
			return nFlags == TF_VISION_FILTER_PYRO ? true : false;
		}
		case 2:
		{
			return nFlags == TF_VISION_FILTER_HALLOWEEN ? true : false;
		}
		case 3:
		{
			return nFlags == TF_VISION_FILTER_ROME ? true : false;
		}
	}

	return Hook.Original<FN>()(nFlags, bWeaponsCheck);
}