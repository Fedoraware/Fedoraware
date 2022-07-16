#include "Commands.h"

#include <utility>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>

#include "../../Features/Chams/DMEChams.h"

bool CCommands::Run(const std::string& cmd, const std::deque<std::string>& args)
{
	if (CommandMap.find(cmd) == CommandMap.end()) { return false; }
	CommandMap[cmd](args);

	return true;
}

void CCommands::Register(const std::string& name, CommandCallback callback)
{
	CommandMap[name] = std::move(callback);
}

void CCommands::Init()
{
	Register("queue", [](const std::deque<std::string>& args)
			 {
				 I::TFPartyClient->LoadSavedCasualCriteria();
				 I::TFPartyClient->RequestQueueForMatch(k_eTFMatchGroup_Casual_Default);
			 });

	Register("poop", [](const std::deque<std::string>& args)
			 {
				 if (I::TFGCClientSystem)
				 {
					 *(reinterpret_cast<bool*>(I::TFGCClientSystem) + 1440) = !*(reinterpret_cast<bool*>(I::TFGCClientSystem) + 1440);
					 I::CVars->ConsolePrintf("%d", *(reinterpret_cast<bool*>(I::TFGCClientSystem) + 1440));
				 }
			 });

	Register("crash", [](const std::deque<std::string>& args)
			 {
				 reinterpret_cast<void(__thiscall*)(void*, void*, void*, void*)>(I::Client + 10000)(NULL, NULL, NULL, NULL);
			 });

	Register("setcvar", [](std::deque<std::string> args)
			 {
// Check if the user provided at least 2 args
				 if (args.size() < 2)
				 {
					 I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Usage: setcvar <cvar> <value>\n");
					 return;
				 }

				 // Find the given CVar
				 const auto foundCVar = I::CVars->FindVar(args[0].c_str());
				 const std::string cvarName = args[0];
				 if (!foundCVar)
				 {
					 I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Could not find %s\n", cvarName.c_str());
					 return;
				 }

				 // Set the CVar to the given value
				 args.pop_front();
				 std::string newValue = boost::algorithm::join(args, " ");
				 boost::replace_all(newValue, "\"", "");
				 foundCVar->SetValue(newValue.c_str());
				 I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Set %s to: %s\n", cvarName.c_str(), newValue.c_str());
			 });

	Register("getcvar", [](std::deque<std::string> args)
			 {
// Check if the user provided 1 arg
				 if (args.size() != 1)
				 {
					 I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Usage: getcvar <cvar>\n");
					 return;
				 }

				 const auto foundCVar = I::CVars->FindVar(args[0].c_str());
				 const std::string cvarName = args[0];
				 if (!foundCVar)
				 {
					 I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Could not find %s\n", cvarName.c_str());
					 return;
				 }

				 I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Value of %s is: %s\n", cvarName.c_str(), foundCVar->GetString());
			 });

	Register("f_dumpmaterials", [](const std::deque<std::string>& args)
			 {
				 for (IMaterial* scanMat : F::DMEChams.v_MatList)
				 {
					 if (!scanMat) { continue; }

					 const char* matName = scanMat->GetName();
					 const char* matGroupName = scanMat->GetTextureGroupName();
					 const char* GetShaderName = scanMat->GetShaderName();
					 std::vector<std::pair<const char*, bool>> bFlags;
					 std::vector<std::pair<const char*, int>> iFlags;
					 std::vector<std::pair<const char*, float>> fFlags;
					 IMaterialVar** aParam = scanMat->GetShaderParams();
					 {
						 bFlags.push_back({ "InMaterialPage", scanMat->InMaterialPage() });
						 bFlags.push_back({ "IsTranslucent", scanMat->IsTranslucent() });
						 bFlags.push_back({ "IsAlphaTested", scanMat->IsAlphaTested() });
						 bFlags.push_back({ "IsVertexLit", scanMat->IsVertexLit() });
						 bFlags.push_back({ "HasProxy", scanMat->HasProxy() });
						 bFlags.push_back({ "UsesEnvCubemap", scanMat->UsesEnvCubemap() });
						 bFlags.push_back({ "NeedsTangentSpace", scanMat->NeedsTangentSpace() });
						 bFlags.push_back({ "NeedsPowerOfTwoFrameBufferTexture", scanMat->NeedsPowerOfTwoFrameBufferTexture() });
						 bFlags.push_back({ "NeedsFullFrameBufferTexture", scanMat->NeedsFullFrameBufferTexture() });
						 bFlags.push_back({ "NeedsSoftwareSkinning", scanMat->NeedsSoftwareSkinning() });
						 bFlags.push_back({ "IsTwoSided", scanMat->IsTwoSided() });
						 bFlags.push_back({ "NeedsLightmapBlendAlpha", scanMat->NeedsLightmapBlendAlpha() });
						 bFlags.push_back({ "NeedsSoftwareLighting", scanMat->NeedsSoftwareLighting() });
						 bFlags.push_back({ "IsErrorMaterial", scanMat->IsErrorMaterial() });
						 bFlags.push_back({ "IsSpriteCard", scanMat->IsSpriteCard() });
						 bFlags.push_back({ "WasReloadedFromWhitelist", scanMat->WasReloadedFromWhitelist() });
						 bFlags.push_back({ "IsPrecached", scanMat->IsPrecached() });
						 iFlags.push_back({ "GetMappingWidth", scanMat->GetMappingWidth() });
						 iFlags.push_back({ "GetMappingHeight", scanMat->GetMappingHeight() });
						 iFlags.push_back({ "GetNumAnimationFrames", scanMat->GetNumAnimationFrames() });
						 iFlags.push_back({ "GetEnumerationID", scanMat->GetEnumerationID() });
						 iFlags.push_back({ "GetNumPasses", scanMat->GetNumPasses() });
						 iFlags.push_back({ "GetTextureMemoryBytes", scanMat->GetTextureMemoryBytes() });
						 iFlags.push_back({ "ShaderParamCount", scanMat->ShaderParamCount() });
						 fFlags.push_back({ "GetAlphaModulation", scanMat->GetAlphaModulation() });
					 }

					 I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s\n", matName);
					 I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s\n", matGroupName);
					 I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s\n", GetShaderName);
					 for (std::pair<const char*, bool> flag : bFlags)
					 {
						 I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s : %s\n", flag.first, flag.second ? "TRUE" : "FALSE");
					 }
					 for (std::pair<const char*, bool> flag : iFlags)
					 {
						 I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s : %d\n", flag.first, flag.second);
					 }
					 for (std::pair<const char*, bool> flag : fFlags)
					 {
						 I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s : %.1f\n", flag.first, flag.second);
					 }
					 for (int idxParam = 0; idxParam < scanMat->ShaderParamCount(); ++idxParam)
					 {
						 if (!aParam[idxParam])
						 {
							 continue;
						 }

						 I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%s : %s\n", aParam[idxParam]->GetName(), aParam[idxParam]->GetStringValue());
					 }
					 for (int flagIndex = -1; flagIndex < 31; flagIndex++)
					 {
						 const bool bOn = scanMat->GetMaterialVarFlag(static_cast<EMaterialVarFlags>(flagIndex));
						 I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "%d : %s\n", flagIndex, bOn ? "TRUE" : "FALSE");
					 }
					 scanMat->Refresh();
				 }
			 });
}