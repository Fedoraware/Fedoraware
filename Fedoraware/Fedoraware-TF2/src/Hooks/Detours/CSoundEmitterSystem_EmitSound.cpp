#include "../Hooks.h"
#include <boost/algorithm/string.hpp>

const static std::vector<std::string> NOISEMAKER_SOUNDS{ "items/halloween", "items/football_manager", "items/japan_fundraiser", "items/samurai", "items/summer", "misc/happy_birthday_tf", "misc/jingle_bells" };
// TODO: Do the names contain / or \ ?

MAKE_HOOK(CSoundEmitterSystem_EmitSound, g_Pattern.Find(L"client.dll", L"55 8B EC 53 56 8B 75 10 8B D9 57 8B 56 04 85 D2 0F 84 ? ? ? ? 68 ? ? ? ? 52 E8 ? ? ? ? 83 C4 08 85 C0 75 20 68 ? ? ? ? FF 76 04 E8 ? ? ? ?"), void, __fastcall,
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