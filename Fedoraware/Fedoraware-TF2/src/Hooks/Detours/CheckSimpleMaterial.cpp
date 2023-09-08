#include "../Hooks.h"

MAKE_HOOK(CheckSimpleMaterial, S::CheckSimpleMaterial(), bool, __cdecl, IMaterial* pMaterial)
{
	return true;	//	no need to check as it's only ever used for anti-cheat
}
