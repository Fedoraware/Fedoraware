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
	GetVecPatches().push_back(&antiDelayedEvents);
}

void CPatchManager::Restore()
{
	for (const auto& patch : GetVecPatches())
	{
		patch->Restore();
	}

	GetVecPatches().clear();
}