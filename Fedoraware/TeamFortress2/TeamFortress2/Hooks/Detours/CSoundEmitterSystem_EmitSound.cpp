#include "../Hooks.h"
#include <boost/algorithm/string.hpp>

const static std::vector<std::string> NOISEMAKER_SOUNDS{ "items/halloween", "items/football_manager", "items/japan_fundraiser", "items/samurai", "items/summer", "misc/happy_birthday_tf", "misc/jingle_bells" };
// TODO: Do the names contain / or \ ?

MAKE_HOOK(CSoundEmitterSystem_EmitSound, g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? 8D 8D 7C FF FF FF E8 ? ? ? ? 8D 4D C4"), void, __fastcall,
		  void* ecx, void* edx, IRecipientFilter& filter, int entindex, const EmitSound_t& ep)
{
	if (ep.m_pSoundName)
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
	}

	return Hook.Original<FN>()(ecx, edx, filter, entindex, ep);
}