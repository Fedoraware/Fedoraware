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
		return -1;
	}
	}
}

Chams_t getChamsType(int nIndex) {
	switch (nIndex) {
	case 0: {
		return Vars::Chams::DME::Weapon;
	}
	case 1: {
		return Vars::Chams::DME::Hands;
	}
	//case 2: {
	//	//return GetPlayerChams()
	//}
	//case 3: {
	//	//return Vars::Chams::DME::Ragdolls
	//}
	default:
		return Chams_t();
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

		// filter weapon draws
		if (!drawType)
		{
			std::string_view szModelName(I::ModelInfo->GetModelName(pInfo.m_pModel));
			if (!(szModelName.find(_("weapon")) != std::string_view::npos
				&& szModelName.find(_("arrow")) == std::string_view::npos
				&& szModelName.find(_("w_syringe")) == std::string_view::npos
				&& szModelName.find(_("nail")) == std::string_view::npos
				&& szModelName.find(_("shell")) == std::string_view::npos
				&& szModelName.find(_("parachute")) == std::string_view::npos
				&& szModelName.find(_("buffbanner")) == std::string_view::npos
				&& szModelName.find(_("shogun_warbanner")) == std::string_view::npos
				&& szModelName.find(_("targe")) == std::string_view::npos //same as world model, can't filter
				&& szModelName.find(_("shield")) == std::string_view::npos //same as world model, can't filter
				&& szModelName.find(_("repair_claw")) == std::string_view::npos)) {
				return false;
			}
		}
		if (drawType == -1) { return false; }

		Chams_t chams = getChamsType(drawType);
		
		// I wanted these to be 1 line each leave me alone.
		const int proxyIndex = (drawType <= 1) ? (drawType ? Vars::Chams::DME::HandsProxySkin.Value : Vars::Chams::DME::WeaponsProxySkin.Value) : 0;
		const bool proxyWF = (drawType <= 1) ? (drawType ? Vars::Chams::DME::HandsProxyWF.Value : Vars::Chams::DME::WeaponsProxyWF.Value) : false;

		// handle
		{
			const bool rainbow = chams.rainbow;
			const bool rainbowOverlay = chams.overlayRainbow;

			IMaterial* chamsMaterial = GetChamMaterial(chams.drawMaterial);
			I::ModelRender->ForcedMaterialOverride(chamsMaterial);

			if (chams.drawMaterial != 7)
			{
				I::RenderView->SetColorModulation(
					Color::TOFLOAT(rainbow ? Utils::Rainbow().r : chams.colour.r),
					Color::TOFLOAT(rainbow ? Utils::Rainbow().g : chams.colour.g),
					Color::TOFLOAT(rainbow ? Utils::Rainbow().b : chams.colour.b));
			}
			else
			{
				bool found1, found2;
				IMaterialVar* fresnelSelfillumtint = chamsMaterial->FindVar(_("$selfillumtint"), &found2);
				if (found2)
				{
					fresnelSelfillumtint->SetVecValue(
						Color::TOFLOAT(chams.fresnelBase.r) * 4,
						Color::TOFLOAT(chams.fresnelBase.g) * 4,
						Color::TOFLOAT(chams.fresnelBase.b) * 4);
				}
				IMaterialVar* envmap = chamsMaterial->FindVar(_("$envmaptint"), &found1);
				if (found1)
				{
					envmap->SetVecValue(
						Color::TOFLOAT(chams.colour.r) * 4,
						Color::TOFLOAT(chams.colour.g) * 4,
						Color::TOFLOAT(chams.colour.b) * 4);
				}
			}

			I::RenderView->SetBlend(Color::TOFLOAT(chams.colour.a));

			if (dmeHook) {
				dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
			}


			if (proxyIndex)
			{

				IMaterial* pMaterial = GetProxyMaterial(proxyIndex);
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, proxyWF);
				I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				I::ModelRender->ForcedMaterialOverride(pMaterial);

				if (dmeHook) {
					dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
				}
			}

			if (chams.overlayType)
			{
				// Overlay
				IMaterial* pMaterial = m_pMatOverlay;

				IMaterialVar* phongtint = pMaterial->FindVar(_("$phongtint"), nullptr);
				if (phongtint)
				{
					phongtint->SetVecValue(
						Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().r : chams.overlayColour.r),
						Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().g : chams.overlayColour.g),
						Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().b : chams.overlayColour.b));
				}
				IMaterialVar* envmaptint = pMaterial->FindVar(_("$envmaptint"), nullptr);
				if (envmaptint)
				{
					envmaptint->SetVecValue(
						Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().r : chams.overlayColour.r),
						Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().g : chams.overlayColour.g),
						Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().b : chams.overlayColour.b));
				}
				IMaterialVar* phongfresnelranges = pMaterial->FindVar("$phongfresnelranges", nullptr);
				if (phongfresnelranges)
				{
					phongfresnelranges->SetVecValue(0, 0.5 / chams.overlayIntensity, 10 / chams.overlayIntensity);
				}
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chams.overlayType == 2);

				I::RenderView->SetBlend(chams.overlayPulse ? sin(I::GlobalVars->curtime * 5) * 0.5f + 0.51f : Color::TOFLOAT(chams.overlayColour.a));
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
		m_bRendering = false;
	}

	return false;
}
