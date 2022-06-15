#include "DMEChams.h"

#include "../Vars.h"
#include "Chams.h"
#include "../Glow/Glow.h"
#include "../Backtrack/Backtrack.h"
#include "../../Hooks/HookManager.h"
#include "../../Hooks/Hooks.h"


// I can't believe i'm doing this

// TODO: add an option to import custom ones from a file path

namespace ProxySkins
{
	static IMaterial* spectrumSplattered = nullptr;
	static IMaterial* electroSkullsBlue = nullptr;
	static IMaterial* frozenAurora = nullptr;
	static IMaterial* jazzy = nullptr;
	static IMaterial* hana = nullptr;
	static IMaterial* wtf = nullptr;
	static IMaterial* ghost = nullptr;
	static IMaterial* flames = nullptr;
	static IMaterial* spookwood = nullptr;
	static IMaterial* edgy = nullptr;
	static IMaterial* serenity = nullptr;
	static IMaterial* fade = nullptr;

	static KeyValues* Proxies = new KeyValues("Proxies");

	IMaterial* CreateProxySkin(const char* materialPath, const char* materialName) {
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", materialPath);
		kv->SetString("$additive", "1");
		kv->SetString("$scrollanglevar", "0");
		kv->SetString("$wireframe", "0");
		kv->AddSubkey(Proxies);
		return I::MatSystem->Create(tfm::format("DMEProxy_%s", materialName).c_str(), kv);
	}

	void Init()
	{
		{	// initialise proxies subkey
			g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
			{
				KeyValues* Sine = new KeyValues("Sine");
				g_KeyValUtils.Initialize(Sine, (char*)"sine");
				Sine->SetString("resultvar", "$scrollanglevar");
				Sine->SetString("sineperiod", "50000");
				Sine->SetString("sinemin", "0");
				Sine->SetString("sinemax", "360");
				Proxies->AddSubkey(Sine);
			}
			{
				KeyValues* TextureScroll = new KeyValues("TextureScroll");
				g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
				TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
				TextureScroll->SetString("texturescrollrate", "0.1");
				TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
				Proxies->AddSubkey(TextureScroll);
			}
		}

		spectrumSplattered = CreateProxySkin("patterns/workshop/screamfortress_2020/2203845847/2203845847_spectrum", "spectrumSplattered");
		electroSkullsBlue = CreateProxySkin("patterns/workshop/screamfortress_2018/1326382486/1326382486_electro_skulls_blu", "electroSkullsBlue");
		frozenAurora = CreateProxySkin("patterns/workshop/smissmas_2017/1193300219/1193300219_cow", "frozenAurora");
		jazzy = CreateProxySkin("patterns/workshop/smissmas_2020/1558054217/1558054217_a", "jazzy");
		wtf = CreateProxySkin("patterns/workshop/screamfortress_2021/2596228713/2596228713_main_blu", "wtf");
		ghost = CreateProxySkin("patterns/workshop/screamfortress_2021/2594850983/2594850983_pattern_red", "ghost");
		flames = CreateProxySkin("patterns/workshop/screamfortress_2020/2223065529/2223065529_helldriver_flames", "flames");
		spookwood = CreateProxySkin("patterns/workshop/screamfortress_2018/1180839618/1180839618_spooky_wood", "spookwood");
		edgy = CreateProxySkin("patterns/workshop/500642896/500642896", "edgy");
		serenity = CreateProxySkin("patterns/workshop/smissmas_2020/2265040710/2265040710_starlight2", "serenity");
		fade = CreateProxySkin("patterns/workshop/smissmas_2020/2265040710/2265040710_serenity", "fade");
	}
}

bool CDMEChams::ShouldRun()
{
	if (!Vars::Chams::DME::Active.Value || I::EngineVGui->IsGameUIVisible() || !Vars::Chams::Main::Active.
		Value)
		return false;

	return true;
}

void CDMEChams::Init()
{
	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatShaded = I::MatSystem->Create("DME_MAT_m_pMatShaded", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatShiny = I::MatSystem->Create("DME_MAT_m_pMatShiny", kv);
	}

	{
		auto kv = new KeyValues("UnlitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		m_pMatFlat = I::MatSystem->Create("DME_MAT_m_pMatFlat", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$envmap", "skybox/sky_dustbowl_01");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", "[0 0.05 0.1]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[0.5 0.5 0]");
		kv->SetString("$selfillumtint", "[0 0 0]");
		kv->SetString("$envmaptint", "[1 1 1]");
		m_pMatFresnel = I::MatSystem->Create("DME_MAT_m_pMatFresnel", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "brick/brickwall031b");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$color2", "[10 10 10]");
		kv->SetString("$additive", "1");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$envmaptint", "[4 4 0]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$rimlight", "1");
		kv->SetString("$rimlightboost", "10");
		m_pMatBrick = I::MatSystem->Create("DME_MAT_m_pMatBrick", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "models/player/shared/ice_player");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$translucent", "1");
		kv->SetString("$additive", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", "[0 0.5 10]");
		kv->SetString("$phongtint", "[0 0 0]");
		kv->SetString("$envmap", "skybox/sky_dustbowl_01");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$envmaptint", "[0 0 0]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumtint", "[0 0 0]");
		kv->SetString("$rimlight", "1");
		kv->SetString("$rimlightboost", "-5");
		kv->SetString("$wireframe", "0");
		m_pMatOverlay = I::MatSystem->Create("DME_MAT_m_pMatScuffed", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatWFShaded = I::MatSystem->Create("DME_MAT_m_pMatWFShaded", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatWFShiny = I::MatSystem->Create("DME_MAT_m_pMatWFShiny", kv);
	}

	{
		auto kv = new KeyValues("UnlitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		m_pMatWFFlat = I::MatSystem->Create("DME_MAT_m_pMatWFFlat", kv);
	}

	ProxySkins::Init();
}

IMaterial* CDMEChams::GetChamMaterial(int nIndex) {
	switch (nIndex)
	{
	case 1:
	{
		return m_pMatShaded;
	}
	case 2:
	{
		return m_pMatShiny;
	}
	case 3:
	{
		return m_pMatFlat;
	}
	case 4:
	{
		return m_pMatWFShaded;
	}
	case 5:
	{
		return m_pMatWFShiny;
	}
	case 6:
	{
		return m_pMatWFFlat;
	}
	case 7:
	{
		return m_pMatFresnel;
	}
	case 8:
	{
		return m_pMatBrick;
	}
	default: return nullptr;
	}
}

IMaterial* CDMEChams::GetProxyMaterial(int nIndex) {
	using namespace ProxySkins;
	switch (nIndex) {
	case 1:
		return spectrumSplattered;
	case 2:
		return electroSkullsBlue;
	case 3:
		return frozenAurora;
	case 4:
		return jazzy;
	case 5:
		return hana;
	case 6:
		return wtf;
	case 7:
		return ghost;
	case 8:
		return flames;
	case 9:
		return spookwood;
	case 10:
		return edgy;
	case 11:
		return serenity;
	case 12:
		return fade;
	default:
		return nullptr;
	}
}

int GetType(int EntIndex) {
	CBaseEntity* pEntity = I::EntityList->GetClientEntity(EntIndex);
	if (!pEntity) { return 0; }
	switch (pEntity->GetClassID()) {
	case ETFClassID::CTFViewModel: {
		return 1;
	}
	case ETFClassID::CBasePlayer:
	case ETFClassID::CTFPlayer: {
		return 2;
	}
	case ETFClassID::CTFRagdoll: {
		return 3;
	}
	default: {
		return 0;
	}
	}
}

bool CDMEChams::Render(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld)
{
	const auto dmeHook = g_HookManager.GetMapHooks()["ModelRender_DrawModelExecute"];

	m_bRendering = false;
	if (ShouldRun())
	{
		m_bRendering = true;

		const int drawType = GetType(pInfo.m_nEntIndex);

		if (drawType == 1)
		{
			const int chamsIndex = Vars::Chams::DME::Hands.Value;
			const bool rainbow = Vars::Chams::DME::HandsRainbow.Value;

			IMaterial* chamsMaterial = GetChamMaterial(chamsIndex);
			I::ModelRender->ForcedMaterialOverride(chamsMaterial);

			if (chamsIndex != 7)
			{
				I::RenderView->SetColorModulation(
					Color::TOFLOAT(rainbow ? Utils::Rainbow().r : Colors::Hands.r),
					Color::TOFLOAT(rainbow ? Utils::Rainbow().g : Colors::Hands.g),
					Color::TOFLOAT(rainbow ? Utils::Rainbow().b : Colors::Hands.b));
			}
			else
			{
				bool found1, found2;
				IMaterialVar* fresnelSelfillumtint = chamsMaterial->FindVar(_("$selfillumtint"), &found2);
				if (found2)
				{
					fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBaseHands.r) * 4,
						Color::TOFLOAT(Colors::FresnelBaseHands.g) * 4,
						Color::TOFLOAT(Colors::FresnelBaseHands.b) * 4);
				}
				IMaterialVar* envmap = chamsMaterial->FindVar(_("$envmaptint"), &found1);
				if (found1)
				{
					envmap->SetVecValue(Color::TOFLOAT(Colors::Hands.r) * 4, Color::TOFLOAT(Colors::Hands.g) * 4,
						Color::TOFLOAT(Colors::Hands.b) * 4);
				}
			}

			I::RenderView->SetBlend(Color::TOFLOAT(Colors::Hands.a));
			
			if (dmeHook) {
				dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
			}


			if (Vars::Chams::DME::HandsProxySkin.Value)
			{

				IMaterial* pMaterial = GetProxyMaterial(Vars::Chams::DME::HandsProxySkin.Value);

				auto pVar = pMaterial->FindVar("$wireframe", nullptr);
				if (pVar) {
					pVar->SetIntValue(Vars::Chams::DME::HandsProxyWF.Value);
				}
				I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				I::ModelRender->ForcedMaterialOverride(pMaterial);

				if (dmeHook) {
					dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
				}
			}

			if (Vars::Chams::DME::HandsGlowOverlay.Value)
			{
				// Overlay
				IMaterial* pMaterial = m_pMatOverlay;

				IMaterialVar* phongtint = pMaterial->FindVar(_("$phongtint"), nullptr);
				if (phongtint)
				{
					phongtint->SetVecValue(
						Color::TOFLOAT(rainbow ? Utils::Rainbow().r : Colors::HandsOverlay.r),
						Color::TOFLOAT(rainbow ? Utils::Rainbow().g : Colors::HandsOverlay.g),
						Color::TOFLOAT(rainbow ? Utils::Rainbow().b : Colors::HandsOverlay.b));
				}
				IMaterialVar* envmaptint = pMaterial->FindVar(_("$envmaptint"), nullptr);
				if (envmaptint)
				{
					envmaptint->SetVecValue(
						Color::TOFLOAT(rainbow ? Utils::Rainbow().r : Colors::HandsOverlay.r),
						Color::TOFLOAT(rainbow ? Utils::Rainbow().g : Colors::HandsOverlay.g),
						Color::TOFLOAT(rainbow ? Utils::Rainbow().b : Colors::HandsOverlay.b));
				}
				IMaterialVar* phongfresnelranges = pMaterial->FindVar("$phongfresnelranges", nullptr);
				if (phongfresnelranges)
				{
					phongfresnelranges->SetVecValue(0, 0.5 / Vars::Chams::DME::HandsGlowAmount.Value, 10 / Vars::Chams::DME::HandsGlowAmount.Value);
				}
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Vars::Chams::DME::HandsGlowOverlay.Value == 2);

				I::RenderView->SetBlend(Vars::Chams::DME::HandsOverlayPulse.Value ? sin(I::GlobalVars->curtime * 5) * 0.5f + 0.51f : Color::TOFLOAT(Colors::HandsOverlay.a));
				I::ModelRender->ForcedMaterialOverride(pMaterial);

				if (dmeHook) {
					dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
				}
			}


			I::ModelRender->ForcedMaterialOverride(nullptr);
			I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

			I::RenderView->SetBlend(1.0f);

			return true;
		}

		if (!drawType && pInfo.m_pModel)
		{
			std::string_view szModelName(I::ModelInfo->GetModelName(pInfo.m_pModel));

			if (szModelName.find(_("weapon")) != std::string_view::npos
				&& szModelName.find(_("arrow")) == std::string_view::npos
				&& szModelName.find(_("w_syringe")) == std::string_view::npos
				&& szModelName.find(_("nail")) == std::string_view::npos
				&& szModelName.find(_("shell")) == std::string_view::npos
				&& szModelName.find(_("parachute")) == std::string_view::npos
				&& szModelName.find(_("buffbanner")) == std::string_view::npos
				&& szModelName.find(_("shogun_warbanner")) == std::string_view::npos
				&& szModelName.find(_("targe")) == std::string_view::npos //same as world model, can't filter
				&& szModelName.find(_("shield")) == std::string_view::npos //same as world model, can't filter
				&& szModelName.find(_("repair_claw")) == std::string_view::npos)
			{
				//I::DebugOverlay->AddTextOverlay(pInfo.m_vOrigin, 0.003f, "%hs", szModelName);

				bool bMatWasForced = false;

				int _case = 0;

				if (Vars::Chams::DME::Weapon.Value)
				{
					I::ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
						{
							switch (Vars::Chams::DME::Weapon.Value)
							{
							case 1:
							{
								bMatWasForced = true;
								return m_pMatShaded;
							}
							case 2:
							{
								bMatWasForced = true;
								return m_pMatShiny;
							}
							case 3:
							{
								bMatWasForced = true;
								return m_pMatFlat;
							}
							case 4:
							{
								bMatWasForced = true;
								return m_pMatWFShaded;
							}
							case 5:
							{
								bMatWasForced = true;
								return m_pMatWFShiny;
							}
							case 6:
							{
								bMatWasForced = true;
								return m_pMatWFFlat;
							}
							case 7:
							{
								bMatWasForced = true;
								return m_pMatFresnel;
							}
							case 8:
							{
								bMatWasForced = true;
								return m_pMatBrick;
							}
							default: return nullptr;
							}
						}());
				}

				if (Vars::Chams::DME::Weapon.Value != 7)
				{
					if (Vars::Chams::DME::WeaponRainbow.Value)
					{
						I::RenderView->SetColorModulation(Color::TOFLOAT(Utils::Rainbow().r),
							Color::TOFLOAT(Utils::Rainbow().g),
							Color::TOFLOAT(Utils::Rainbow().b));
					}
					else
					{
						I::RenderView->SetColorModulation(Color::TOFLOAT(Colors::Weapon.r),
							Color::TOFLOAT(Colors::Weapon.g),
							Color::TOFLOAT(Colors::Weapon.b));
					}
				}

				if (Vars::Chams::DME::Weapon.Value == 7)
				{
					IMaterial* mat = m_pMatFresnel;
					IMaterialVar* fresnelSelfillumtint = mat->FindVar(_("$selfillumtint"), nullptr);
					if (fresnelSelfillumtint)
					{
						fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBaseWeps.r) * 4,
							Color::TOFLOAT(Colors::FresnelBaseWeps.g) * 4,
							Color::TOFLOAT(Colors::FresnelBaseWeps.b) * 4);
					}
					bool found = false;
					IMaterialVar* envmap = mat->FindVar(_("$envmaptint"), &found);
					if (found)
					{
						envmap->SetVecValue(Color::TOFLOAT(Colors::Weapon.r) * 4, Color::TOFLOAT(Colors::Weapon.g) * 4,
							Color::TOFLOAT(Colors::Weapon.b) * 4);
					}
				}

				I::RenderView->SetBlend(Color::TOFLOAT(Colors::Weapon.a));

				if (dmeHook) {
					dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
				}

				bMatWasForced = true;
				
				if (Vars::Chams::DME::WeaponsProxySkin.Value && bMatWasForced)
				{

					IMaterial* pMaterial = nullptr;
					{
						using namespace ProxySkins;
						switch (Vars::Chams::DME::WeaponsProxySkin.Value) {
						case 1:
							pMaterial = spectrumSplattered;
							break;
						case 2:
							pMaterial = electroSkullsBlue;
							break;
						case 3:
							pMaterial = frozenAurora;
							break;
						case 4:
							pMaterial = jazzy;
							break;
						case 5:
							pMaterial = hana;
							break;
						case 6:
							pMaterial = wtf;
							break;
						case 7:
							pMaterial = ghost;
							break;
						case 8:
							pMaterial = flames;
							break;
						case 9:
							pMaterial = spookwood;
							break;
						case 10:
							pMaterial = edgy;
							break;
						case 11:
							pMaterial = serenity;
							break;
						case 12:
							pMaterial = fade;
							break;
						default: break;
						}
					}
					bool found = false;
					auto pVar = pMaterial->FindVar("$wireframe", &found);
					if (pVar) {
						pVar->SetIntValue(Vars::Chams::DME::WeaponsProxyWF.Value);
					}
					I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
					I::ModelRender->ForcedMaterialOverride(pMaterial);

					if (dmeHook) {
						dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
					}
				}

				if (Vars::Chams::DME::WeaponGlowOverlay.Value && bMatWasForced)
				{
					IMaterial* pMaterial = m_pMatOverlay;
					bool found = false;
					bool found2 = false;
					bool found3 = false;
					IMaterialVar* pVar = pMaterial->FindVar(_("$phongtint"), &found);
					if (found)
					{
						if (Vars::Chams::DME::WeaponOverlayRainbow.Value)
						{
							pVar->SetVecValue(Color::TOFLOAT(Utils::Rainbow().r) * 4,
								Color::TOFLOAT(Utils::Rainbow().g) * 4,
								Color::TOFLOAT(Utils::Rainbow().b) * 4);
						}
						else
						{
							pVar->SetVecValue(Color::TOFLOAT(Colors::WeaponOverlay.r),
								Color::TOFLOAT(Colors::WeaponOverlay.g),
								Color::TOFLOAT(Colors::WeaponOverlay.b));
						}
					}
					IMaterialVar* pVar2 = pMaterial->FindVar(_("$envmaptint"), &found2);
					if (found2)
					{
						if (Vars::Chams::DME::WeaponOverlayRainbow.Value)
						{
							pVar2->SetVecValue(Color::TOFLOAT(Utils::Rainbow().r) * 4,
								Color::TOFLOAT(Utils::Rainbow().g) * 4,
								Color::TOFLOAT(Utils::Rainbow().b) * 4);
						}
						else
						{
							pVar2->SetVecValue(Color::TOFLOAT(Colors::WeaponOverlay.r),
								Color::TOFLOAT(Colors::WeaponOverlay.g),
								Color::TOFLOAT(Colors::WeaponOverlay.b));
						}
					}
					bool foundfresnelranges = false;
					IMaterialVar* phongfresnelranges = pMaterial->FindVar("$phongfresnelranges", &foundfresnelranges);
					if (foundfresnelranges)
					{
						phongfresnelranges->SetVecValue(0, 0.5 / Vars::Chams::DME::WeaponGlowAmount.Value, 10 / Vars::Chams::DME::WeaponGlowAmount.Value);
					}
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Vars::Chams::DME::WeaponGlowOverlay.Value == 2);

					I::RenderView->SetBlend(Vars::Chams::DME::WeaponOverlayPulse.Value ? sin(I::GlobalVars->curtime * 5) * 0.5f + 0.51f : Color::TOFLOAT(Colors::WeaponOverlay.a));
					I::ModelRender->ForcedMaterialOverride(pMaterial);

					if (dmeHook) {
						dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
					}
				}

				if (bMatWasForced)
				{
					I::ModelRender->ForcedMaterialOverride(nullptr);
					I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}

				I::RenderView->SetBlend(1.0f);

				return true;
			}
		}

		m_bRendering = false;
	}

	return false;
}
