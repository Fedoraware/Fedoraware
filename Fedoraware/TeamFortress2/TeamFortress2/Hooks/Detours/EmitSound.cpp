#include "../Hooks.h"
#include <boost/algorithm/string.hpp>

const static std::vector<std::string> NOISEMAKER_SOUNDS{ "items/halloween", "items/football_manager", "items/japan_fundraiser", "items/samurai", "items/summer", "misc/happy_birthday_tf", "misc/jingle_bells" };
// TODO: Do the names contain / or \ ?

DWORD EmitSoundAddress()
{
	const DWORD dwAddress = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 8D 8D 7C FF FF FF E8 ? ? ? ? 8D 4D C4") + 0x1;
	return *reinterpret_cast<PDWORD>(dwAddress) + dwAddress + 4;
}

MAKE_HOOK(EmitSound, EmitSoundAddress(), void, __fastcall, void* ecx, void* edx, IRecipientFilter& filter, int entindex, const EmitSound_t& ep)
{
	std::string soundName(ep.m_pSoundName);
	boost::algorithm::to_lower(soundName);

	// Footsteps
	if (Vars::Misc::SoundBlock.Value & (1 << 0))
	{
		if (soundName.find("footsteps") != std::string::npos)
		{
			return;
		}
	}

	// Noisemaker
	if (Vars::Misc::SoundBlock.Value & (1 << 1))
	{
		for (auto& sound : NOISEMAKER_SOUNDS)
		{
			if (soundName.find(sound) != std::string::npos)
			{
				return;
			}
		}
	}

	return Hook.Original<FN>()(ecx, edx, filter, entindex, ep);
}