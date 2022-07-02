#include "../Hooks.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include <boost/algorithm/string/join.hpp>
#include <numeric>
#include "../../Features/Menu/MaterialEditor/MaterialEditor.h"
#include "../../Features/Chams/DMEChams.h"

class split_q
{
public:
	split_q() : in_q(false) {}

	bool operator()(char ch) const
	{
		if (ch == '\"')
		{
			in_q = !in_q;
		}
		return !in_q && ch == ' ';
	}

private:
	mutable bool in_q;

};

MAKE_HOOK(EngineClient_ClientCmd_Unrestricted, Utils::GetVFuncPtr(I::Engine, 106), void, __fastcall,
		  void* ecx, void* edx, const char* szCmdString)
{
	std::string cmdString(szCmdString);
	std::deque<std::string> cmdArgs;

	// Yes I will use boost for this
	boost::split(cmdArgs, cmdString, split_q());

	if (!cmdArgs.empty())
	{
		const std::string cmdName = cmdArgs.front();
		cmdArgs.pop_front();

		switch (FNV1A::HashConst(cmdName.c_str())) {
		case FNV1A::HashConst("queue"): {
			static auto CTFGCClientSystem__PreInitGC = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"56 8B F1 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 68 ? ? ? ?"));
			static auto CCasualSidePanel__OnCommand = reinterpret_cast<void(__stdcall*)(const char* pCommand)>(g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 8B F9 81 FE ? ? ? ? 74 20 68 ? ? ? ? 56 E8 ? ? ? ? 83 C4 08 85 C0 74 0E 56 8B CF E8 ? ? ? ? 5F 5E 5D C2 04 00 6A 07"));
			static void* m_pSOCache = *reinterpret_cast<void**>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 85 C9 74 3D 68 ? ? ? ?") + 0x1);
			static auto CSharedObjectCache__FindBaseTypeCache = reinterpret_cast<int(__thiscall*)(void*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 8B 45 08 53 56 8B D9 89 45 F8 57 8D 45 F8 50 8D 4B 10 E8 ? ? ? ? 0F B7 F0 8B D6 3B 53 18 73 43 0F B7 43 26 B9 ? ? ? ? 66 3B C1 74 35"));
			static auto CTFParty__SpewDebug = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 68 A1 ? ? ? ? 53 8B D9 89 45 F0 8B 83 ? ? ? ? 8D 4D E8 56 8D B3 ? ? ? ? 89 5D F8 51 8B CE C7 45 ? ? ? ? ? C6 45 F4 00 FF 50 1C 8B C8 E8 ? ? ? ?"));
			static auto GTFGCClientSystem = reinterpret_cast<void* (__cdecl*)()>(g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? 0F B6 CB"));
			static auto CTFGCClientSystem__GetParty = reinterpret_cast<void* (__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8B 89 ? ? ? ? 85 C9 74 1D 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 0F 8B 48 18 85 C9 74 08 8B 40 04 8B 44 88 FC C3"));
			static auto GTFPartyClient = reinterpret_cast<void* (__cdecl*)()>(g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? FF 70 24"));
			static auto CTFPartyClient__LoadSavedCasualCriteria = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"83 79 18 00 C6 81 ? ? ? ? ? 74 06 80 79 28 00 74 07 C6 81 ? ? ? ? ? FF 35 ? ? ? ? 81 C1 ? ? ? ? E8 ? ? ? ? C3"));

			if (auto pPartyClient = GTFPartyClient())
			{
				CTFPartyClient__LoadSavedCasualCriteria(pPartyClient);
				CCasualSidePanel__OnCommand("find_game");
			}

			return;
		}
		case FNV1A::HashConst("argh"): {
			static auto CEconItem__CEconItem = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"53 8B D9 56 57 BE ? ? ? ? C7 03 ? ? ? ? 8D 7B 08 C7 03 ? ? ? ? C7 43 ? ? ? ? ? 6A 01 6A 00 6A 00 8B CF E8 ? ? ? ?"));
			static auto TFInventoryManager = *reinterpret_cast<void**>(g_Pattern.Find(L"client.dll", L"B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ?") + 0x1);
			static auto CInventoryManager_GetItemByBackpackPosition = reinterpret_cast<void* (__thiscall*)(void*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 8B 01"));
			static auto CPlayerInventory__AddEconItem = reinterpret_cast<bool(__thiscall*)(void*, void*, bool, int, bool)>(g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 53 56 8B F1 8D 8D ? ? ? ? 57 E8 ? ? ? ? 8B 7D 08 8D 85 ? ? ? ? 57 50 8B CE E8 ? ? ? ?"));

			return;
		}
		case FNV1A::HashConst("poop"): {
			if (I::TFGCClientSystem)
			{
				*((bool*)I::TFGCClientSystem + 1440) = !*((bool*)I::TFGCClientSystem + 1440);
				I::CVars->ConsolePrintf("%d", *((bool*)I::TFGCClientSystem + 1440));
			}
			return;
		}
		case FNV1A::HashConst("setcvar"): {
			// Check if the user provided at least 2 args
			if (cmdArgs.size() < 2)
			{
				I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Usage: setcvar <cvar> <value>\n");
				return;
			}

			// Find the given CVar
			const auto foundCVar = I::CVars->FindVar(cmdArgs[0].c_str());
			const std::string cvarName = cmdArgs[0];
			if (!foundCVar)
			{
				I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Could not find %s\n", cvarName.c_str());
				return;
			}

			// Set the CVar to the given value
			cmdArgs.pop_front();
			std::string newValue = boost::algorithm::join(cmdArgs, " ");
			boost::replace_all(newValue, "\"", "");
			foundCVar->SetValue(newValue.c_str());
			I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Set %s to %s\n", cvarName.c_str(), newValue.c_str());
			return;
		}
		case FNV1A::HashConst("f_dumpmaterials"): {
			for (IMaterial* scanMat : F::DMEChams.v_MatList) {
				if (!scanMat) { continue; }

				const char* matName = scanMat->GetName();
				const char* matGroupName = scanMat->GetTextureGroupName();
				const char* GetShaderName = scanMat->GetShaderName();
				std::vector<std::pair<const char*, bool>> bFlags;
				std::vector<std::pair<const char*, int>> iFlags;
				std::vector<std::pair<const char*, float>> fFlags;
				IMaterialVar** aParam = scanMat->GetShaderParams();
				{
					bFlags.push_back({ _("InMaterialPage"), scanMat->InMaterialPage() });
					bFlags.push_back({ _("IsTranslucent"), scanMat->IsTranslucent() });
					bFlags.push_back({ _("IsAlphaTested"), scanMat->IsAlphaTested() });
					bFlags.push_back({ _("IsVertexLit"), scanMat->IsVertexLit() });
					bFlags.push_back({ _("HasProxy"), scanMat->HasProxy() });
					bFlags.push_back({ _("UsesEnvCubemap"), scanMat->UsesEnvCubemap() });
					bFlags.push_back({ _("NeedsTangentSpace"), scanMat->NeedsTangentSpace() });
					bFlags.push_back({ _("NeedsPowerOfTwoFrameBufferTexture"), scanMat->NeedsPowerOfTwoFrameBufferTexture() });
					bFlags.push_back({ _("NeedsFullFrameBufferTexture"), scanMat->NeedsFullFrameBufferTexture() });
					bFlags.push_back({ _("NeedsSoftwareSkinning"), scanMat->NeedsSoftwareSkinning() });
					bFlags.push_back({ _("IsTwoSided"), scanMat->IsTwoSided() });
					bFlags.push_back({ _("NeedsLightmapBlendAlpha"), scanMat->NeedsLightmapBlendAlpha() });
					bFlags.push_back({ _("NeedsSoftwareLighting"), scanMat->NeedsSoftwareLighting() });
					bFlags.push_back({ _("IsErrorMaterial"), scanMat->IsErrorMaterial() });
					bFlags.push_back({ _("IsSpriteCard"), scanMat->IsSpriteCard() });
					bFlags.push_back({ _("WasReloadedFromWhitelist"), scanMat->WasReloadedFromWhitelist() });
					bFlags.push_back({ _("IsPrecached"), scanMat->IsPrecached() });
					iFlags.push_back({ _("GetMappingWidth"), scanMat->GetMappingWidth() });
					iFlags.push_back({ _("GetMappingHeight"), scanMat->GetMappingHeight() });
					iFlags.push_back({ _("GetNumAnimationFrames"), scanMat->GetNumAnimationFrames() });
					iFlags.push_back({ _("GetEnumerationID"), scanMat->GetEnumerationID() });
					iFlags.push_back({ _("GetNumPasses"), scanMat->GetNumPasses() });
					iFlags.push_back({ _("GetTextureMemoryBytes"), scanMat->GetTextureMemoryBytes() });
					iFlags.push_back({ _("ShaderParamCount"), scanMat->ShaderParamCount() });
					fFlags.push_back({ _("GetAlphaModulation"), scanMat->GetAlphaModulation() });
				}

				I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s\n", matName);
				I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s\n", matGroupName);
				I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s\n", GetShaderName);
				for (std::pair<const char*, bool> flag : bFlags) {
					I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s : %s\n", flag.first, flag.second ? "TRUE" : "FALSE");
				}
				for (std::pair<const char*, bool> flag : iFlags) {
					I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s : %d\n", flag.first, flag.second);
				}
				for (std::pair<const char*, bool> flag : fFlags) {
					I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s : %.1f\n", flag.first, flag.second);
				}
				for (int idxParam = 0; idxParam < scanMat->ShaderParamCount(); ++idxParam)
				{
					if (!aParam[idxParam]) {
						continue;
					}

					I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s : %s\n", aParam[idxParam]->GetName(), aParam[idxParam]->GetStringValue());
				}
				scanMat->Refresh();
			}
			return;
		}
		}
	}

	Hook.Original<FN>()(ecx, edx, cmdString.c_str());
}