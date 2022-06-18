#include "DMEChams.h"

#include "../Vars.h"
#include "Chams.h"
#include "../Menu/MaterialEditor/MaterialEditor.h"
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
		hana = CreateProxySkin("patterns/workshop/smissmas_2017/1183962036/1183962036_aes_blue", "hana");
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

IMaterial* CreateNRef(char const* szName, void* pKV) {
	IMaterial* returnMaterial = I::MatSystem->Create(szName, pKV);
	returnMaterial->IncrementReferenceCount();
	return returnMaterial;
}

void CDMEChams::Init()
{
	static KeyValues* m_pMatShadedkv = new KeyValues("VertexLitGeneric");
	static KeyValues* m_pMatShinykv = new KeyValues("VertexLitGeneric");
	static KeyValues* m_pMatFlatkv = new KeyValues("UnlitGeneric");
	static KeyValues* m_pMatFresnelkv = new KeyValues("VertexLitGeneric");
	static KeyValues* m_pMatBrickkv = new KeyValues("VertexLitGeneric");
	static KeyValues* m_pMatOverlaykv = new KeyValues("VertexLitGeneric");
	static KeyValues* m_pMatWFShadedkv = new KeyValues("VertexLitGeneric");
	static KeyValues* m_pMatWFShinykv = new KeyValues("VertexLitGeneric");
	static KeyValues* m_pMatWFFlatkv = new KeyValues("UnlitGeneric");

	static bool setup = false;

	if (!setup)
	{
		m_pMatShadedkv->SetString("$basetexture", "vgui/white_additive");
		m_pMatShadedkv->SetString("$bumpmap", "vgui/white_additive");
		m_pMatShadedkv->SetString("$selfillum", "1");
		m_pMatShadedkv->SetString("$selfillumfresnel", "1");
		m_pMatShadedkv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");

		m_pMatShinykv->SetString("$basetexture", "vgui/white_additive");
		m_pMatShinykv->SetString("$bumpmap", "vgui/white_additive");
		m_pMatShinykv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		m_pMatShinykv->SetString("$envmaptint", "[1 1 1]");
		m_pMatShinykv->SetString("$selfillum", "1");
		m_pMatShinykv->SetString("$selfillumfresnel", "1");
		m_pMatShinykv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");

		m_pMatFlatkv->SetString("$basetexture", "vgui/white_additive");

		m_pMatFresnelkv->SetString("$basetexture", "vgui/white_additive");
		m_pMatFresnelkv->SetString("$bumpmap", "models/player/shared/shared_normal");
		m_pMatFresnelkv->SetString("$envmap", "skybox/sky_dustbowl_01");
		m_pMatFresnelkv->SetString("$envmapfresnel", "1");
		m_pMatFresnelkv->SetString("$phong", "1");
		m_pMatFresnelkv->SetString("$phongfresnelranges", "[0 0.05 0.1]");
		m_pMatFresnelkv->SetString("$selfillum", "1");
		m_pMatFresnelkv->SetString("$selfillumfresnel", "1");
		m_pMatFresnelkv->SetString("$selfillumfresnelminmaxexp", "[0.5 0.5 0]");
		m_pMatFresnelkv->SetString("$selfillumtint", "[0 0 0]");
		m_pMatFresnelkv->SetString("$envmaptint", "[1 1 1]");

		m_pMatBrickkv->SetString("$basetexture", "brick/brickwall031b");
		m_pMatBrickkv->SetString("$bumpmap", "vgui/white_additive");
		m_pMatBrickkv->SetString("$color2", "[10 10 10]");
		m_pMatBrickkv->SetString("$additive", "1");
		m_pMatBrickkv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		m_pMatBrickkv->SetString("$envmapfresnel", "1");
		m_pMatBrickkv->SetString("$envmaptint", "[1 1 1]");
		m_pMatBrickkv->SetString("$selfillum", "1");
		m_pMatBrickkv->SetString("$rimlight", "1");
		m_pMatBrickkv->SetString("$rimlightboost", "10");

		m_pMatOverlaykv->SetString("$basetexture", "models/player/shared/ice_player");
		m_pMatOverlaykv->SetString("$bumpmap", "models/player/shared/shared_normal");
		m_pMatOverlaykv->SetString("$translucent", "1");
		m_pMatOverlaykv->SetString("$additive", "1");
		m_pMatOverlaykv->SetString("$phong", "1");
		m_pMatOverlaykv->SetString("$phongfresnelranges", "[0 0.5 10]");
		m_pMatOverlaykv->SetString("$phongtint", "[0 0 0]");
		m_pMatOverlaykv->SetString("$envmap", "skybox/sky_dustbowl_01");
		m_pMatOverlaykv->SetString("$envmapfresnel", "1");
		m_pMatOverlaykv->SetString("$envmaptint", "[1 1 1]");
		m_pMatOverlaykv->SetString("$selfillum", "1");
		m_pMatOverlaykv->SetString("$selfillumtint", "[0 0 0]");
		m_pMatOverlaykv->SetString("$rimlight", "1");
		m_pMatOverlaykv->SetString("$rimlightboost", "-5");
		m_pMatOverlaykv->SetString("$wireframe", "0");

		m_pMatWFShadedkv->SetString("$wireframe", "1");
		m_pMatWFShadedkv->SetString("$basetexture", "vgui/white_additive");
		m_pMatWFShadedkv->SetString("$bumpmap", "vgui/white_additive");
		m_pMatWFShadedkv->SetString("$selfillum", "1");
		m_pMatWFShadedkv->SetString("$selfillumfresnel", "1");
		m_pMatWFShadedkv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");

		m_pMatWFShinykv->SetString("$wireframe", "1");
		m_pMatWFShinykv->SetString("$basetexture", "vgui/white_additive");
		m_pMatWFShinykv->SetString("$bumpmap", "vgui/white_additive");
		m_pMatWFShinykv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		m_pMatWFShinykv->SetString("$envmaptint", "[1 1 1]");
		m_pMatWFShinykv->SetString("$selfillum", "1");
		m_pMatWFShinykv->SetString("$selfillumfresnel", "1");
		m_pMatWFShinykv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");

		m_pMatWFFlatkv->SetString("$wireframe", "1");
		m_pMatWFFlatkv->SetString("$basetexture", "vgui/white_additive");

		setup = true;
	}


	m_pMatShaded = CreateNRef("DME_MAT_m_pMatShaded", m_pMatShadedkv);
	m_pMatShiny = CreateNRef("DME_MAT_m_pMatShiny", m_pMatShinykv);
	m_pMatFlat = CreateNRef("DME_MAT_m_pMatFlat", m_pMatFlatkv);
	m_pMatFresnel = CreateNRef("DME_MAT_m_pMatFresnel", m_pMatFresnelkv);
	m_pMatBrick = CreateNRef("DME_MAT_m_pMatBrick", m_pMatBrickkv);
	m_pMatOverlay = CreateNRef("DME_MAT_m_pMatScuffed", m_pMatOverlaykv);
	m_pMatWFShaded = CreateNRef("DME_MAT_m_pMatWFShaded", m_pMatWFShadedkv);
	m_pMatWFShiny = CreateNRef("DME_MAT_m_pMatWFShiny", m_pMatWFShinykv);
	m_pMatWFFlat = CreateNRef("DME_MAT_m_pMatWFFlat", m_pMatWFFlatkv);

	ProxySkins::Init();
}

IMaterial* CDMEChams::GetChamMaterial(const Chams_t& chams) {
	switch (chams.drawMaterial)
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
	case 9:
	{
		return F::MaterialEditor.GetByName(chams.customMaterial);
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
	case ETFClassID::CTFWearable: {
		return 4;
	}
	case ETFClassID::CTFAmmoPack: {
		return 6;
	}
	case ETFClassID::CBaseAnimating: {
		const auto szName = pEntity->GetModelName();

		if (Hash::IsAmmo(szName))
		{
			return 6;
		}

		if (Hash::IsHealth(szName))
		{
			return 7;
		}

		break;
	}
	case ETFClassID::CObjectSentrygun:
	case ETFClassID::CObjectDispenser:
	case ETFClassID::CObjectTeleporter: {
		return 8;
	}
	case ETFClassID::CTFProjectile_Rocket:
	case ETFClassID::CTFGrenadePipebombProjectile:
	case ETFClassID::CTFProjectile_Jar:
	case ETFClassID::CTFProjectile_JarGas:
	case ETFClassID::CTFProjectile_JarMilk:
	case ETFClassID::CTFProjectile_Arrow:
	case ETFClassID::CTFProjectile_SentryRocket:
	case ETFClassID::CTFProjectile_Flare:
	case ETFClassID::CTFProjectile_Cleaver:
	case ETFClassID::CTFProjectile_EnergyBall:
	case ETFClassID::CTFProjectile_HealingBolt:
	case ETFClassID::CTFProjectile_ThrowableBreadMonster: {
		return 9;
	}
	}
	CBaseCombatWeapon* pWeapon = reinterpret_cast<CBaseCombatWeapon*>(pEntity);
	if (pWeapon) {
		return 5;
	}
	return -1;
}

Chams_t GetPlayerChams(CBaseEntity* pEntity) {
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (pEntity && pLocal)
	{
		if (pEntity->GetIndex() == G::CurrentTargetIdx && Vars::Chams::Players::Target.chamsActive) {
			return Vars::Chams::Players::Target;
		}
		if (pEntity == pLocal) {
			return Vars::Chams::Players::Local;
		}
		if (g_EntityCache.IsFriend(pEntity->GetIndex()) && Vars::Chams::Players::Friend.chamsActive) {
			return Vars::Chams::Players::Friend;
		}
		if (pEntity->GetTeamNum() != pLocal->GetTeamNum()) {
			return Vars::Chams::Players::Enemy;
		}
		if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) {
			return Vars::Chams::Players::Team;
		}
	}
	return Chams_t();
}

Chams_t GetBuildingChams(CBaseEntity* pEntity) {
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (pEntity && pLocal)
	{
		if (pEntity->GetIndex() == G::CurrentTargetIdx && Vars::Chams::Buildings::Target.chamsActive) {
			return Vars::Chams::Buildings::Target;
		}
		if (pEntity->GetIndex() == pLocal->GetIndex()) {
			return Vars::Chams::Buildings::Local;
		}
		if (g_EntityCache.IsFriend(pEntity->GetIndex()) && Vars::Chams::Buildings::Friend.chamsActive) {
			return Vars::Chams::Buildings::Friend;
		}
		if (pEntity->GetTeamNum() != pLocal->GetTeamNum()) {
			return Vars::Chams::Buildings::Enemy;
		}
		if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) {
			return Vars::Chams::Buildings::Team;
		}
	}
	return Chams_t();
}

Chams_t getChamsType(int nIndex, CBaseEntity* pEntity = nullptr) {
	switch (nIndex) {
	case 0: {
		return Vars::Chams::DME::Weapon;
	}
	case 1: {
		return Vars::Chams::DME::Hands;
	}
	case 2: {
		return pEntity ? GetPlayerChams(pEntity) : Chams_t();
	}
	case 3: {
		return Vars::Chams::Players::Ragdoll;
	}
	case 4: {
		if (!Vars::Chams::Players::Wearables.Value) { return Chams_t(); }
		if (!pEntity) { return Chams_t(); }
		if (CBaseEntity* pOwner = I::EntityList->GetClientEntityFromHandle(pEntity->m_hOwnerEntity())) {
			return GetPlayerChams(pOwner);
		}
		return Chams_t();
	}
	case 5: {
		if (!Vars::Chams::Players::Weapons.Value) { return Chams_t(); }
		if (!pEntity) { return Chams_t(); }
		if (CBaseEntity* pOwner = I::EntityList->GetClientEntityFromHandle(pEntity->m_hOwnerEntity())) {
			return GetPlayerChams(pOwner);
		}
		else if (int teamNum = pEntity->GetTeamNum()) {	// if we don't have an owner, we need to do this, or else spawned buildings that do have a team will return no cham struct.
			CBaseEntity* pLocal = g_EntityCache.GetLocal();
			if (pLocal) {
				return (teamNum = pLocal->GetTeamNum()) ? Vars::Chams::Buildings::Team : Vars::Chams::Buildings::Enemy;
			}
		}
		return Chams_t();
	}
	case 6: {
		return Vars::Chams::World::Ammo;
	}
	case 7: {
		return Vars::Chams::World::Health;
	}
	case 8: {
		if (!pEntity) { return Chams_t(); }
		const auto& Building = reinterpret_cast<CBaseObject*>(pEntity);
		if (!Building || !(!Building->GetCarried() && Building->GetConstructed())) { return Chams_t(); }
		if (CBaseEntity* pOwner = Building->GetOwner()) {
			return GetBuildingChams(pOwner);
		}
		return Chams_t();
	}
	case 9: {
		if (!pEntity) { return Chams_t(); }
		if (CBaseEntity* pOwner = I::EntityList->GetClientEntityFromHandle(reinterpret_cast<int>(pEntity->GetThrower()))) {
			return GetPlayerChams(pOwner);
		}
		return Chams_t();
	}
	default:
		return Chams_t();
	}
}

bool CDMEChams::Render(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld)
{
	const auto dmeHook = g_HookManager.GetMapHooks()["ModelRender_DrawModelExecute"];
	const auto& pRenderContext = I::MatSystem->GetRenderContext();

	m_bRendering = false;
	if (ShouldRun() && pRenderContext)
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

		CBaseEntity* pEntity = I::EntityList->GetClientEntity(pInfo.m_nEntIndex);
		CBaseEntity* pLocal = g_EntityCache.GetLocal();

		Chams_t chams = pEntity ? getChamsType(drawType, pEntity) : getChamsType(drawType);

		if (!chams.chamsActive) { return false; }
		
		// I wanted these to be 1 line each leave me alone.
		const int proxyIndex = (drawType <= 1) ? (drawType ? Vars::Chams::DME::HandsProxySkin.Value : Vars::Chams::DME::WeaponsProxySkin.Value) : 0;
		const bool proxyWF = (drawType <= 1) ? (drawType ? Vars::Chams::DME::HandsProxyWF.Value : Vars::Chams::DME::WeaponsProxyWF.Value) : false;

		// handle
		{
			const bool rainbow = chams.rainbow;
			const bool rainbowOverlay = chams.overlayRainbow;

			IMaterial* chamsMaterial = GetChamMaterial(chams);

			if (chamsMaterial) {
				chamsMaterial->IncrementReferenceCount();
			}

			pRenderContext->DepthRange(0.0f, chams.showObstructed ? 0.2f : 1.f);

			I::ModelRender->ForcedMaterialOverride(chamsMaterial);

			if (chamsMaterial == m_pMatFresnel)
			{
				if (IMaterialVar* $envmaptint = chamsMaterial->FindVar(_("$envmaptint"), nullptr, false)) {
					$envmaptint->SetVecValue(
						Color::TOFLOAT(chams.colour.r) * 4,
						Color::TOFLOAT(chams.colour.g) * 4,
						Color::TOFLOAT(chams.colour.b) * 4);
				}
				if (IMaterialVar* $selfillumtint = chamsMaterial->FindVar(_("$selfillumtint"), nullptr, false)) {
					$selfillumtint->SetVecValue(
						Color::TOFLOAT(chams.fresnelBase.r) * 4,
						Color::TOFLOAT(chams.fresnelBase.g) * 4,
						Color::TOFLOAT(chams.fresnelBase.b) * 4);
				}
			}
			else {
				I::RenderView->SetColorModulation(
					Color::TOFLOAT(rainbow ? Utils::Rainbow().r : chams.colour.r),
					Color::TOFLOAT(rainbow ? Utils::Rainbow().g : chams.colour.g),
					Color::TOFLOAT(rainbow ? Utils::Rainbow().b : chams.colour.b));
			}

			float alpha = Color::TOFLOAT(chams.colour.a);
			if (pEntity && pLocal) {
				if (drawType == 2 && pEntity != pLocal && pEntity->GetTeamNum() == pLocal->GetTeamNum() && pLocal->IsAlive() && Vars::Chams::Players::FadeoutTeammates.Value) {
					alpha = Math::RemapValClamped(pLocal->GetWorldSpaceCenter().DistTo(pEntity->GetWorldSpaceCenter()), 450.f, 100.f, Color::TOFLOAT(chams.colour.a), 0.0f);
				}
			}
			I::RenderView->SetBlend(alpha);


			if (dmeHook) {
				dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
			}


			if (proxyIndex)
			{
				IMaterial* pMaterial = GetProxyMaterial(proxyIndex);
				if (pMaterial) {

					pMaterial->IncrementReferenceCount();

					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, proxyWF);
					I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
					I::ModelRender->ForcedMaterialOverride(pMaterial);
				}

				if (dmeHook) {
					dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
				}
			}

			if (chams.overlayType)
			{
				// Overlay
				IMaterial* pMaterial = m_pMatOverlay;

				if (pMaterial) {
					pMaterial->IncrementReferenceCount();

					if (IMaterialVar* $phongtint = pMaterial->FindVar(_("$phongtint"), nullptr, false))
					{
						$phongtint->SetVecValue(
							Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().r : chams.overlayColour.r),
							Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().g : chams.overlayColour.g),
							Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().b : chams.overlayColour.b));
					}
					if (IMaterialVar* $envmaptint = pMaterial->FindVar(_("$envmaptint"), nullptr, false))
					{
						$envmaptint->SetVecValue(
							Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().r : chams.overlayColour.r),
							Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().g : chams.overlayColour.g),
							Color::TOFLOAT(rainbowOverlay ? Utils::Rainbow().b : chams.overlayColour.b));
					}
					if (IMaterialVar* $phongfresnelranges = pMaterial->FindVar("$phongfresnelranges", nullptr, false))
					{
						$phongfresnelranges->SetVecValue(0, 0.5 / chams.overlayIntensity, 10 / chams.overlayIntensity);
					}
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chams.overlayType == 2);

					I::RenderView->SetBlend(chams.overlayPulse ? sin(I::GlobalVars->curtime * 5) * 0.5f + 0.51f : Color::TOFLOAT(chams.overlayColour.a));
					I::ModelRender->ForcedMaterialOverride(pMaterial);
				}

				if (dmeHook) {
					dmeHook->Original<void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>()(I::ModelRender, pState, pInfo, pBoneToWorld);
				}
			}

			pRenderContext->DepthRange(0.0f, 1.f);
			I::ModelRender->ForcedMaterialOverride(nullptr);
			I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

			I::RenderView->SetBlend(1.0f);

			return true;
		}
		m_bRendering = false;
	}

	return false;
}
