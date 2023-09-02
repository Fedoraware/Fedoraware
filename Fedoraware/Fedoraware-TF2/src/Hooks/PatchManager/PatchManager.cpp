#include "PatchManager.h"

#include "../../Utils/Pattern/Pattern.h"
#include "../../Utils/BytePatch/BytePatch.hpp"

void CPatchManager::Init()
{
	Restore();
	GetVecPatches().clear();

	// Initialize all patches

	// https://github.com/OthmanAba/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/engine/cl_parse_event.cpp#L97
	static BytePatch antiDelayedEvents{ g_Pattern.Find(L"engine.dll", L"7B 17 B9 ? ? ? ?"), { 0xEB } };
	static BytePatch skyBoxFix{ g_Pattern.Find(L"engine.dll", L"0F 87 ? ? ? ? 8B 04 9D ? ? ? ? 8B 14 85 ? ? ? ? 85 D2 0F 84 ? ? ? ? 8B 06"), {0x90, 0x90, 0x90, 0x90, 0x90, 0x90} };
	static BytePatch cloakedSpyCheckRemoval{ g_Pattern.Find(L"client.dll", L"74 7F 8B 07 8B CF FF 90 ? ? ? ? 8B 4D F8 8B F0 8B 11 FF 92 ? ? ? ? 3B C6 74 64 8B 75 F8 8B CE 6A 39 E8 ? ? ? ? 8B C8 85 C9 74 1D 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 0F 80 BE"), {0xEB} };
	//static BytePatch resetLatched{ g_Pattern.Find(L"client.dll", L"74 ? 8B 47 ? 8D 4F ? FF 90 ? ? ? ? 5F"), {0xEB} };
	//static BytePatch resetLatchedAnimating{ g_Pattern.Find(L"client.dll", L"74 ? 8B 47 ? 8D 4F ? FF 90 ? ? ? ? 80 BF"), {0xEB} };
	//static BytePatch m_nCritSeedRequestsBlock{ g_Pattern.Find(L"client.dll", L"FF 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 80 B8 ? ? ? ? ? 74 0A F3 0F 10 15 ? ? ? ? EB 58 F3 0F 2A"), {0x90} };
	GetVecPatches().push_back(&antiDelayedEvents);
	GetVecPatches().push_back(&skyBoxFix);
	GetVecPatches().push_back(&cloakedSpyCheckRemoval);
	//GetVecPatches().push_back(&resetLatched);
	//GetVecPatches().push_back(&resetLatchedAnimating);
	//GetVecPatches().push_back(&m_nCritSeedRequestsBlock);
}

void CPatchManager::Restore()
{
	for (const auto& patch : GetVecPatches())
	{
		patch->Restore();
	}

	GetVecPatches().clear();
}