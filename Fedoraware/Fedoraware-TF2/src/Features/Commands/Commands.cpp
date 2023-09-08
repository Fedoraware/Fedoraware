#include "Commands.h"

#include <utility>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>

#include "../../Features/Chams/DMEChams.h"

bool CCommands::Run(const std::string& cmd, const std::deque<std::string>& args)
{
	if (!CommandMap.contains(cmd)) { return false; }
	CommandMap[cmd](args);

	return true;
}

void CCommands::Register(const std::string& name, CommandCallback callback)
{
	CommandMap[name] = std::move(callback);
}

void CCommands::Error(const std::string& msg)
{
	I::Cvar->ConsoleColorPrintf({230, 75, 60, 255}, (msg + "\n").c_str());
}

void CCommands::Init()
{
	Register("queue", [](const std::deque<std::string>& args) {
		I::TFPartyClient->LoadSavedCasualCriteria();
		I::TFPartyClient->RequestQueueForMatch(k_eTFMatchGroup_Casual_Default);
	});

	Register("poop", [](const std::deque<std::string>& args) {
		if (I::TFGCClientSystem)
		{
			*(reinterpret_cast<bool*>(I::TFGCClientSystem) + 1440) = !*(reinterpret_cast<bool*>(I::TFGCClientSystem) + 1440);
			I::Cvar->ConsolePrintf("%d", *(reinterpret_cast<bool*>(I::TFGCClientSystem) + 1440));
		}
	});

	Register("crash", [](const std::deque<std::string>& args) {
		reinterpret_cast<void(__thiscall*)(void*, void*, void*, void*)>(I::BaseClientDLL + 10000)(nullptr, nullptr, nullptr, nullptr);
	});

	Register("setcvar", [](std::deque<std::string> args) {
		// Check if the user provided at least 2 args
		if (args.size() < 2)
		{
			I::Cvar->ConsoleColorPrintf({255, 255, 255, 255}, "Usage: setcvar <cvar> <value>\n");
			return;
		}

		// Find the given CVar
		const auto foundCVar = I::Cvar->FindVar(args[0].c_str());
		const std::string cvarName = args[0];
		if (!foundCVar)
		{
			I::Cvar->ConsoleColorPrintf({255, 255, 255, 255}, "Could not find %s\n", cvarName.c_str());
			return;
		}

		// Set the CVar to the given value
		args.pop_front();
		std::string newValue = boost::algorithm::join(args, " ");
		boost::replace_all(newValue, "\"", "");
		foundCVar->SetValue(newValue.c_str());
		I::Cvar->ConsoleColorPrintf({255, 255, 255, 255}, "Set %s to: %s\n", cvarName.c_str(), newValue.c_str());
	});

	Register("getcvar", [](const std::deque<std::string>& args) {
		// Check if the user provided 1 arg
		if (args.size() != 1)
		{
			I::Cvar->ConsoleColorPrintf({255, 255, 255, 255}, "Usage: getcvar <cvar>\n");
			return;
		}

		const auto foundCVar = I::Cvar->FindVar(args[0].c_str());
		const std::string& cvarName = args[0];
		if (!foundCVar)
		{
			I::Cvar->ConsoleColorPrintf({255, 255, 255, 255}, "Could not find %s\n", cvarName.c_str());
			return;
		}

		I::Cvar->ConsoleColorPrintf({255, 255, 255, 255}, "Value of %s is: %s\n", cvarName.c_str(), foundCVar->GetString());
	});

	Register("f_hash", [](const std::deque<std::string>& args) {
		constexpr std::hash<std::string_view> m_Hash;
		const char* hashString = args[0].c_str();
		I::Cvar->ConsolePrintf("%lu\n", m_Hash(hashString));
	});

	Register("f_dumpmaterials", [](const std::deque<std::string>& args) {
		for (IMaterial* scanMat : F::DMEChams.v_MatList)
		{
			if (!scanMat) { continue; }

			const char* matName = scanMat->GetName();
			const char* matGroupName = scanMat->GetTextureGroupName();
			const char* shaderName = scanMat->GetShaderName();
			std::vector<std::pair<const char*, bool>> bFlags;
			std::vector<std::pair<const char*, int>> iFlags;
			std::vector<std::pair<const char*, float>> fFlags;
			IMaterialVar** aParam = scanMat->GetShaderParams();
			{
				bFlags.emplace_back("InMaterialPage", scanMat->InMaterialPage());
				bFlags.emplace_back("IsTranslucent", scanMat->IsTranslucent());
				bFlags.emplace_back("IsAlphaTested", scanMat->IsAlphaTested());
				bFlags.emplace_back("IsVertexLit", scanMat->IsVertexLit());
				bFlags.emplace_back("HasProxy", scanMat->HasProxy());
				bFlags.emplace_back("UsesEnvCubemap", scanMat->UsesEnvCubemap());
				bFlags.emplace_back("NeedsTangentSpace", scanMat->NeedsTangentSpace());
				bFlags.emplace_back("NeedsPowerOfTwoFrameBufferTexture", scanMat->NeedsPowerOfTwoFrameBufferTexture());
				bFlags.emplace_back("NeedsFullFrameBufferTexture", scanMat->NeedsFullFrameBufferTexture());
				bFlags.emplace_back("NeedsSoftwareSkinning", scanMat->NeedsSoftwareSkinning());
				bFlags.emplace_back("IsTwoSided", scanMat->IsTwoSided());
				bFlags.emplace_back("NeedsLightmapBlendAlpha", scanMat->NeedsLightmapBlendAlpha());
				bFlags.emplace_back("NeedsSoftwareLighting", scanMat->NeedsSoftwareLighting());
				bFlags.emplace_back("IsErrorMaterial", scanMat->IsErrorMaterial());
				bFlags.emplace_back("IsSpriteCard", scanMat->IsSpriteCard());
				bFlags.emplace_back("WasReloadedFromWhitelist", scanMat->WasReloadedFromWhitelist());
				bFlags.emplace_back("IsPrecached", scanMat->IsPrecached());
				iFlags.emplace_back("GetMappingWidth", scanMat->GetMappingWidth());
				iFlags.emplace_back("GetMappingHeight", scanMat->GetMappingHeight());
				iFlags.emplace_back("GetNumAnimationFrames", scanMat->GetNumAnimationFrames());
				iFlags.emplace_back("GetEnumerationID", scanMat->GetEnumerationID());
				iFlags.emplace_back("GetNumPasses", scanMat->GetNumPasses());
				iFlags.emplace_back("GetTextureMemoryBytes", scanMat->GetTextureMemoryBytes());
				iFlags.emplace_back("ShaderParamCount", scanMat->ShaderParamCount());
				fFlags.emplace_back("GetAlphaModulation", scanMat->GetAlphaModulation());
			}

			I::Cvar->ConsoleColorPrintf({204, 0, 255, 255}, "%s\n", matName);
			I::Cvar->ConsoleColorPrintf({204, 0, 255, 255}, "%s\n", matGroupName);
			I::Cvar->ConsoleColorPrintf({204, 0, 255, 255}, "%s\n", shaderName);

			for (const std::pair<const char*, bool> flag : bFlags)
			{
				I::Cvar->ConsoleColorPrintf({204, 0, 255, 255}, "%s : %s\n", flag.first, flag.second ? "TRUE" : "FALSE");
			}

			for (const std::pair<const char*, bool> flag : iFlags)
			{
				I::Cvar->ConsoleColorPrintf({204, 0, 255, 255}, "%s : %d\n", flag.first, flag.second);
			}

			for (const std::pair<const char*, bool> flag : fFlags)
			{
				I::Cvar->ConsoleColorPrintf({204, 0, 255, 255}, "%s : %.1f\n", flag.first, flag.second);
			}

			for (int idxParam = 0; idxParam < scanMat->ShaderParamCount(); ++idxParam)
			{
				if (!aParam[idxParam])
				{
					continue;
				}

				I::Cvar->ConsoleColorPrintf({204, 0, 255, 255}, "%s : %s\n", aParam[idxParam]->GetName(), aParam[idxParam]->GetStringValue());
			}

			for (int flagIndex = -1; flagIndex < 31; flagIndex++)
			{
				const bool bOn = scanMat->GetMaterialVarFlag(static_cast<EMaterialVarFlags>(flagIndex));
				I::Cvar->ConsoleColorPrintf({204, 0, 255, 255}, "%d : %s\n", flagIndex, bOn ? "TRUE" : "FALSE");
			}

			scanMat->Refresh();
		}
	});
}
