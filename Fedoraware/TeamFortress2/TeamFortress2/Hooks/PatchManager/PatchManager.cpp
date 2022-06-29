#include "PatchManager.h"

#include "../../Utils/Pattern/Pattern.h"
#include "../../Utils/BytePatch/BytePatch.hpp"

void CPatchManager::Init()
{
	Restore();
	GetVecPatches().clear();

	// Initialize all patches

	// https://github.com/OthmanAba/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/engine/cl_parse_event.cpp#L97
	static BytePatch antiDelayedEvents{ g_Pattern.Find(L"engine.dll", L"7B 17 B9 ? ? ? ?"), { 0xEB }};
	static BytePatch skyBoxFix{ g_Pattern.Find(L"engine.dll", L"0F 87 ? ? ? ? 8B 04 9D ? ? ? ? 8B 14 85 ? ? ? ? 85 D2 0F 84 ? ? ? ? 8B 06 "), {0x90, 0x90, 0x90, 0x90, 0x90, 0x90} };
	GetVecPatches().push_back(&antiDelayedEvents);
	GetVecPatches().push_back(&skyBoxFix);
}

void CPatchManager::Restore()
{
	for (const auto& patch : GetVecPatches())
	{
		patch->Restore();
	}

	GetVecPatches().clear();
}