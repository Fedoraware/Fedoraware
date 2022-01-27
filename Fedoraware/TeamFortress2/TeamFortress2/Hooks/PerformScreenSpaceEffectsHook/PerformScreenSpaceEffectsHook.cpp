#include "PerformScreenSpaceEffectsHook.h"

void __stdcall PerformScreenSpaceEffects::Hook(int x, int y, int w, int h)
{
	//afaik only game's glow is here right?
	//if so fuck that, we got our own...
	return;
}