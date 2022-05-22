#include "PatchManager.h"

#include "../../Utils/BytePatch/BytePatch.hpp"

void CPatchManager::Init()
{
	// Initialize all patches
}

void CPatchManager::Restore()
{
	for (const auto& patch : GetVecPatches())
	{
		patch->Restore();
	}
}