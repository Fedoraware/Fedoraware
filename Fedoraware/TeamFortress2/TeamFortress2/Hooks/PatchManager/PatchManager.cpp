#include "PatchManager.h"

#include "../../Utils/Pattern/Pattern.h"
#include "../../Utils/BytePatch/BytePatch.hpp"

void CPatchManager::Init()
{
	Restore();
	GetVecPatches().clear();

	// Initialize all patches

	// Add them to the patch vector
}

void CPatchManager::Restore()
{
	for (const auto& patch : GetVecPatches())
	{
		patch->Restore();
	}

	GetVecPatches().clear();
}