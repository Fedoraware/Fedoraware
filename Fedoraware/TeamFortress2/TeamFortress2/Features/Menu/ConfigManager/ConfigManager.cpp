#include "ConfigManager.h"
#include "ConfigManager.h"

#include <string>

#include "../../Vars.h"
#include "../../../SDK/SDK.h"
#include "../../Radar/Radar.h"
#include "../../SpectatorList/SpectatorList.h"
#include "../../Keybinds/Keybinds.h"

#define SAVE_VAR(x) Save(_(L#x), x.m_Var)
#define LOAD_VAR(x) Load(_(L#x), x.m_Var)

#define SAVE_STRING(x) Save(_(L#x), x)
#define LOAD_STRING(x) Load(_(L#x), x)

#define SAVE_OTHER(x) Save(_(L#x), x)
#define LOAD_OTHER(x) Load(_(L#x), x)

//stfu
#pragma warning (disable : 6328)
#pragma warning (disable : 6031)
#pragma warning (disable : 4477)

bool CConfigManager::Find(const wchar_t *name, std::wstring &output)
{
	m_Read.clear();
	m_Read.seekg(0, std::ios::beg);

	while (std::getline(m_Read, output))
	{
		if (output.find(name) != std::wstring::npos)
			return true;
	}

	return false;
}

//int cum = XorStr("hello").Encrypt();

void CConfigManager::Save(const wchar_t *name, bool val)
{
	char buffer[64];
	sprintf_s(buffer, "%ls: %d", name, val);
	m_Write << buffer << "\n";
}

void CConfigManager::Save(const wchar_t* name, std::string val)
{
	char buffer[128];
	sprintf_s(buffer, "%ls: %s", name, val.c_str());
	m_Write << buffer << "\n";
}

void CConfigManager::Save(const wchar_t *name, int val)
{
	char buffer[64];
	sprintf_s(buffer, "%ls: %d", name, val);
	m_Write << buffer << "\n";
}

void CConfigManager::Save(const wchar_t *name, float val)
{
	char buffer[64];
	sprintf_s(buffer, "%ls: %f", name, val);
	m_Write << buffer << "\n";
}

void CConfigManager::Save(const wchar_t *name, Color_t val)
{
	char buffer[64];
	sprintf_s(buffer, "%ls: %d %d %d %d", name, val.r, val.g, val.b, val.a);
	m_Write << buffer << "\n";
}

void CConfigManager::Load(const wchar_t* name, std::string& val)
{
	std::wstring line = {};

	if (Find(name, line)) {
		//swscanf_s(line.c_str(), L"%*ls:");
		std::wstring delimiter = L": ";
		std::wstring svalue = line.substr(line.find(L": ") + 2, sizeof(line));
		std::string stringvalue(svalue.begin(), svalue.end());
		val = stringvalue;
	}
}

void CConfigManager::Load(const wchar_t *name, bool &val)
{
	std::wstring line = {};

	if (Find(name, line))
		swscanf_s(line.c_str(), L"%*ls %d", &val);
}

void CConfigManager::Load(const wchar_t *name, int &val)
{
	std::wstring line = {};

	if (Find(name, line))
		swscanf_s(line.c_str(), L"%*ls %d", &val);
}

void CConfigManager::Load(const wchar_t *name, float &val)
{
	std::wstring line = {};

	if (Find(name, line))
		swscanf_s(line.c_str(), L"%*ls %f", &val);
}

void CConfigManager::Load(const wchar_t *name, Color_t &val)
{
	std::wstring line = {};

	if (Find(name, line)) {
		int r = 0, g = 0, b = 0, a = 0;
		swscanf_s(line.c_str(), L"%*ls %d %d %d %d", &r, &g, &b, &a);
		val = { static_cast<byte>(r), static_cast<byte>(g), static_cast<byte>(b), static_cast<byte>(a) };
	}
}

CConfigManager::CConfigManager()
{
	m_sConfigPath = std::filesystem::current_path().wstring() + _(L"\\FedFigs");

	if (!std::filesystem::exists(m_sConfigPath))
		std::filesystem::create_directory(m_sConfigPath);

	if (!std::filesystem::exists(m_sConfigPath + _(L"\\FedCore")))
		std::filesystem::create_directory(m_sConfigPath + _(L"\\FedCore"));
}

void CConfigManager::Save(const wchar_t *name)
{
	m_Write = std::wofstream(m_sConfigPath + L"\\" + name + _(L".fed"));

	if (m_Write.is_open())
	{
		//Aimbot
		{
			//Global
			{
				SAVE_VAR(Vars::Aimbot::Global::Active);
				SAVE_VAR(Vars::Aimbot::Global::AimKey);
				SAVE_VAR(Vars::Aimbot::Global::AimFOV);
				SAVE_VAR(Vars::Aimbot::Global::AutoShoot);
				SAVE_VAR(Vars::Aimbot::Global::AimPlayers);
				SAVE_VAR(Vars::Aimbot::Global::AimBuildings);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreInvlunerable);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreCloaked);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreFriends);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreTaunting);
				SAVE_VAR(Vars::Aimbot::Global::BAimLethal);
				SAVE_VAR(Vars::Aimbot::Global::showHitboxes);
				SAVE_VAR(Vars::Aimbot::Global::clearPreviousHitbox);
				SAVE_VAR(Vars::Aimbot::Global::hitboxTime);
			}

			{
				SAVE_VAR(Vars::Crits::Active);
				SAVE_VAR(Vars::Crits::CritKey);
			}

			//Hitscan
			{
				//SAVE_VAR(Vars::Aimbot::Hitscan::Active);
				SAVE_VAR(Vars::Aimbot::Hitscan::SortMethod);
				SAVE_VAR(Vars::Aimbot::Hitscan::AimMethod);
				SAVE_VAR(Vars::Aimbot::Hitscan::AimHitbox);
				//SAVE_VAR(Vars::Aimbot::Hitscan::AimFOV);
				SAVE_VAR(Vars::Aimbot::Hitscan::SmoothingAmount);
				SAVE_VAR(Vars::Aimbot::Hitscan::TapFire);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScanHitboxes);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScanHead);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScanBuildings);
				SAVE_VAR(Vars::Aimbot::Hitscan::WaitForHeadshot);
				SAVE_VAR(Vars::Aimbot::Hitscan::WaitForCharge);
				SAVE_VAR(Vars::Aimbot::Hitscan::SpectatedSmooth);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScopedOnly);
				SAVE_VAR(Vars::Aimbot::Hitscan::AutoScope);
				SAVE_VAR(Vars::Aimbot::Hitscan::AutoRev);
				
			}

			//Projectile
			{
				//SAVE_VAR(Vars::Aimbot::Projectile::Active);
				SAVE_VAR(Vars::Aimbot::Projectile::PerformanceMode);
				SAVE_VAR(Vars::Aimbot::Projectile::SortMethod);
				SAVE_VAR(Vars::Aimbot::Projectile::AimMethod);
				SAVE_VAR(Vars::Aimbot::Projectile::AimPosition);
				SAVE_VAR(Vars::Aimbot::Projectile::FeetAimIfOnGround);
				SAVE_VAR(Vars::Aimbot::Projectile::ManualZAdjust);
				SAVE_VAR(Vars::Aimbot::Projectile::ZAdjustAmount);
				//SAVE_VAR(Vars::Aimbot::Projectile::AimFOV);
			}

			//Melee
			{
				//SAVE_VAR(Vars::Aimbot::Melee::Active);
				SAVE_VAR(Vars::Aimbot::Melee::SortMethod);
				SAVE_VAR(Vars::Aimbot::Melee::AimMethod);
				//SAVE_VAR(Vars::Aimbot::Melee::AimFOV);
				SAVE_VAR(Vars::Aimbot::Melee::SmoothingAmount);
				SAVE_VAR(Vars::Aimbot::Melee::RangeCheck);
				SAVE_VAR(Vars::Aimbot::Melee::PredictSwing);
			}
		}

		//Triggerbot
		{
			//Global
			{
				SAVE_VAR(Vars::Triggerbot::Global::Active);
				SAVE_VAR(Vars::Triggerbot::Global::TriggerKey);
				SAVE_VAR(Vars::Triggerbot::Global::IgnoreInvlunerable);
				SAVE_VAR(Vars::Triggerbot::Global::IgnoreCloaked);
				SAVE_VAR(Vars::Triggerbot::Global::IgnoreFriends);
			}

			//Shoot
			{
				SAVE_VAR(Vars::Triggerbot::Shoot::Active);
				SAVE_VAR(Vars::Triggerbot::Shoot::TriggerPlayers);
				SAVE_VAR(Vars::Triggerbot::Shoot::TriggerBuildings);
				SAVE_VAR(Vars::Triggerbot::Shoot::HeadOnly);
				SAVE_VAR(Vars::Triggerbot::Shoot::WaitForCharge);
				SAVE_VAR(Vars::Triggerbot::Shoot::HeadScale);
			}

			//Stab
			{
				SAVE_VAR(Vars::Triggerbot::Stab::Active);
				SAVE_VAR(Vars::Triggerbot::Stab::RageMode);
				SAVE_VAR(Vars::Triggerbot::Stab::Silent);
				SAVE_VAR(Vars::Triggerbot::Stab::Disguise);
				SAVE_VAR(Vars::Triggerbot::Stab::IgnRazor);
				SAVE_VAR(Vars::Triggerbot::Stab::Range);
			}

			//Detonate
			{
				SAVE_VAR(Vars::Triggerbot::Detonate::Active);
				SAVE_VAR(Vars::Triggerbot::Detonate::Stickies);
				SAVE_VAR(Vars::Triggerbot::Detonate::Flares);
				SAVE_VAR(Vars::Triggerbot::Detonate::RadiusScale);
			}

			//Blast
			{
				SAVE_VAR(Vars::Triggerbot::Blast::Active);
				SAVE_VAR(Vars::Triggerbot::Blast::Rage);
				SAVE_VAR(Vars::Triggerbot::Blast::Silent);
				SAVE_VAR(Vars::Triggerbot::Blast::Fov);
			}

			//Uber
			{
				SAVE_VAR(Vars::Triggerbot::Uber::Active);
				SAVE_VAR(Vars::Triggerbot::Uber::OnlyFriends);
				SAVE_VAR(Vars::Triggerbot::Uber::PopLocal);
				SAVE_VAR(Vars::Triggerbot::Uber::AutoVacc);
				SAVE_VAR(Vars::Triggerbot::Uber::HealthLeft);
			}
		}

		//ESP
		{
			//Main
			{
				SAVE_VAR(Vars::ESP::Main::Active);
				SAVE_VAR(Vars::ESP::Main::Outlinedbar);
				SAVE_VAR(Vars::ESP::Main::EnableTeamEnemyColors);
			}

			//Players
			{
				SAVE_VAR(Vars::ESP::Players::Active);
				SAVE_VAR(Vars::ESP::Players::ShowLocal);
				SAVE_VAR(Vars::ESP::Players::IgnoreTeammates);
				SAVE_VAR(Vars::ESP::Players::IgnoreCloaked);
				SAVE_VAR(Vars::ESP::Players::Name);
				SAVE_VAR(Vars::ESP::Players::NameBox);
				SAVE_VAR(Vars::ESP::Players::Uber);
				SAVE_VAR(Vars::ESP::Players::Class);
				SAVE_VAR(Vars::ESP::Players::Health);
				SAVE_VAR(Vars::ESP::Players::Cond);
				SAVE_VAR(Vars::ESP::Players::HealthBar);
				SAVE_VAR(Vars::ESP::Players::Box);
				SAVE_VAR(Vars::ESP::Players::GUID);
				SAVE_VAR(Vars::ESP::Players::Alpha);
				SAVE_VAR(Vars::ESP::Players::Lines);
				SAVE_VAR(Vars::ESP::Players::Bones);
				SAVE_VAR(Vars::ESP::Players::Dlights);
				SAVE_VAR(Vars::ESP::Players::DlightRadius);
				SAVE_VAR(Vars::ESP::Players::Headscale);
				SAVE_VAR(Vars::ESP::Players::Torsoscale);
				SAVE_VAR(Vars::ESP::Players::Handscale);
			}

			//Buildings
			{
				SAVE_VAR(Vars::ESP::Buildings::Active);
				SAVE_VAR(Vars::ESP::Buildings::IgnoreTeammates);
				SAVE_VAR(Vars::ESP::Buildings::Name);
				SAVE_VAR(Vars::ESP::Buildings::NameBox);
				SAVE_VAR(Vars::ESP::Buildings::Health);
				SAVE_VAR(Vars::ESP::Buildings::Owner);
				SAVE_VAR(Vars::ESP::Buildings::Level);
				SAVE_VAR(Vars::ESP::Buildings::Cond);
				SAVE_VAR(Vars::ESP::Buildings::HealthBar);
				SAVE_VAR(Vars::ESP::Buildings::Box);
				SAVE_VAR(Vars::ESP::Buildings::Alpha);
				SAVE_VAR(Vars::ESP::Buildings::Dlights);
				SAVE_VAR(Vars::ESP::Buildings::DlightRadius);
			}

			//World
			{
				SAVE_VAR(Vars::ESP::World::Active);
				SAVE_VAR(Vars::ESP::World::HealthText);
				SAVE_VAR(Vars::ESP::World::AmmoText);
				SAVE_VAR(Vars::ESP::World::Alpha);
			}
		}

		//Chams
		{
			//Main
			{
				SAVE_VAR(Vars::Chams::Main::Active);
			}

			//Players
			{
				SAVE_VAR(Vars::Chams::Players::Active);
				SAVE_VAR(Vars::Chams::Players::ShowLocal);
				SAVE_VAR(Vars::Chams::Players::IgnoreTeammates);
				SAVE_VAR(Vars::Chams::Players::Wearables);
				SAVE_VAR(Vars::Chams::Players::Weapons);
				SAVE_VAR(Vars::Chams::Players::Material);
				SAVE_VAR(Vars::Chams::Players::IgnoreZ);
				SAVE_VAR(Vars::Chams::Players::Alpha);
			}

			//Buildings
			{
				SAVE_VAR(Vars::Chams::Buildings::Active);
				SAVE_VAR(Vars::Chams::Buildings::IgnoreTeammates);
				SAVE_VAR(Vars::Chams::Buildings::Material);
				SAVE_VAR(Vars::Chams::Buildings::IgnoreZ);
				SAVE_VAR(Vars::Chams::Buildings::Alpha);
			}

			//World
			{
				SAVE_VAR(Vars::Chams::World::Active);
				SAVE_VAR(Vars::Chams::World::Health);
				SAVE_VAR(Vars::Chams::World::Ammo);
				SAVE_VAR(Vars::Chams::World::Projectiles);
				SAVE_VAR(Vars::Chams::World::Material);
				SAVE_VAR(Vars::Chams::World::IgnoreZ);
				SAVE_VAR(Vars::Chams::World::Alpha);
			}

			//DME
			{
				SAVE_VAR(Vars::Chams::DME::Active);
				SAVE_VAR(Vars::Chams::DME::Hands);

				SAVE_VAR(Vars::Chams::DME::HandsGlowOverlay);
				SAVE_VAR(Vars::Chams::DME::HandsAlpha);
				SAVE_VAR(Vars::Chams::DME::Weapon);
				SAVE_VAR(Vars::Chams::DME::WeaponAlpha);
				SAVE_VAR(Vars::Chams::DME::WeaponGlowOverlay);
				SAVE_VAR(Vars::Chams::DME::WeaponRimMultiplier);
				SAVE_VAR(Vars::Chams::DME::HandsRimMultiplier);
			}
		}

		//Glow
		{
			//Main
			{
				SAVE_VAR(Vars::Glow::Main::Active);
				SAVE_VAR(Vars::Glow::Main::Scale);
			}

			//Players
			{
				SAVE_VAR(Vars::Glow::Players::Active);
				SAVE_VAR(Vars::Glow::Players::ShowLocal);
				SAVE_VAR(Vars::Glow::Players::LocalRainbow);
				SAVE_VAR(Vars::Glow::Players::IgnoreTeammates);
				SAVE_VAR(Vars::Glow::Players::Wearables);
				SAVE_VAR(Vars::Glow::Players::Weapons);
				SAVE_VAR(Vars::Glow::Players::Alpha);
				SAVE_VAR(Vars::Glow::Players::Color);
			}

			//Buildings
			{
				SAVE_VAR(Vars::Glow::Buildings::Active);
				SAVE_VAR(Vars::Glow::Buildings::IgnoreTeammates);
				SAVE_VAR(Vars::Glow::Buildings::Alpha);
				SAVE_VAR(Vars::Glow::Buildings::Color);
			}

			//World
			{
				SAVE_VAR(Vars::Glow::World::Active);
				SAVE_VAR(Vars::Glow::World::Health);
				SAVE_VAR(Vars::Glow::World::Ammo);
				SAVE_VAR(Vars::Glow::World::Projectiles);
				SAVE_VAR(Vars::Glow::World::Alpha);
			}
		}

		//Radar
		{
			//Main
			{
				SAVE_VAR(Vars::Radar::Main::Active);
				SAVE_VAR(Vars::Radar::Main::BackAlpha);
				SAVE_VAR(Vars::Radar::Main::LineAlpha);
				SAVE_VAR(Vars::Radar::Main::Size);
				SAVE_VAR(Vars::Radar::Main::Range);
			}

			//Players
			{
				SAVE_VAR(Vars::Radar::Players::Active);
				SAVE_VAR(Vars::Radar::Players::IconType);
				SAVE_VAR(Vars::Radar::Players::BackGroundType);
				SAVE_VAR(Vars::Radar::Players::Outline);
				SAVE_VAR(Vars::Radar::Players::IgnoreTeam);
				SAVE_VAR(Vars::Radar::Players::IgnoreCloaked);
				SAVE_VAR(Vars::Radar::Players::Health);
			}

			//Buildings
			{
				SAVE_VAR(Vars::Radar::Buildings::Active);
				SAVE_VAR(Vars::Radar::Buildings::Outline);
				SAVE_VAR(Vars::Radar::Buildings::IgnoreTeam);
				SAVE_VAR(Vars::Radar::Buildings::Health);
			}

			//World
			{
				SAVE_VAR(Vars::Radar::World::Active);
				SAVE_VAR(Vars::Radar::World::Health);
				SAVE_VAR(Vars::Radar::World::Ammo);
			}
		}

		//Visuals
		{
			SAVE_VAR(Vars::Visuals::RemoveDisguises);
			SAVE_VAR(Vars::Visuals::RemoveTaunts);
			SAVE_VAR(Vars::Visuals::FieldOfView);
			SAVE_VAR(Vars::Visuals::AimFOVAlpha);
			SAVE_VAR(Vars::Visuals::RemoveScope);
			SAVE_VAR(Vars::Visuals::ScopeLines);
			SAVE_VAR(Vars::Visuals::RemoveZoom);
			SAVE_VAR(Vars::Visuals::RemovePunch);
			SAVE_VAR(Vars::Visuals::CrosshairAimPos);
			SAVE_VAR(Vars::Visuals::ChatInfo);
			SAVE_VAR(Vars::Visuals::OutOfFOVArrowsOutline);
			SAVE_VAR(Vars::Visuals::SpectatorList);

			SAVE_VAR(Vars::Visuals::SpyWarning);
			SAVE_VAR(Vars::Visuals::SpyWarningAnnounce);
			SAVE_VAR(Vars::Visuals::SpyWarningStyle);
			SAVE_VAR(Vars::Visuals::SpyWarningVisibleOnly);
			SAVE_VAR(Vars::Visuals::SpyWarningIgnoreFriends);

			SAVE_VAR(Vars::Visuals::Snow);
			SAVE_VAR(Vars::Visuals::ToolTips);

			SAVE_VAR(Vars::Visuals::ThirdPerson);
			SAVE_VAR(Vars::Visuals::ThirdPersonKey);
			SAVE_VAR(Vars::Visuals::ThirdPersonSilentAngles);
			SAVE_VAR(Vars::Visuals::ThirdPersonInstantYaw);
			SAVE_VAR(Vars::Visuals::ThirdPersonServerHitbox);
			SAVE_VAR(Vars::Visuals::WorldModulation);
			SAVE_VAR(Vars::Visuals::PropWireframe);
			SAVE_VAR(Vars::Visuals::SkyboxChanger);
			SAVE_VAR(Vars::Visuals::BulletTracer);
			SAVE_VAR(Vars::Visuals::BulletTracerRainbow);
			SAVE_VAR(Vars::Visuals::AimbotViewmodel);
			SAVE_VAR(Vars::Visuals::VMOffX);
			SAVE_VAR(Vars::Visuals::VMOffY);
			SAVE_VAR(Vars::Visuals::VMOffZ);
			SAVE_VAR(Vars::Visuals::VMRoll);
			SAVE_VAR(Vars::Visuals::OutOfFOVArrows);
			SAVE_VAR(Vars::Visuals::ArrowLength);
			SAVE_VAR(Vars::Visuals::ArrowAngle);
			SAVE_VAR(Vars::Visuals::MaxDist);
			SAVE_VAR(Vars::Visuals::MinDist);
			SAVE_VAR(Vars::Visuals::AimPosSquare);
				
			SAVE_VAR(Vars::Visuals::despawnTime);
			SAVE_VAR(Vars::Visuals::damageLogger);
			SAVE_VAR(Vars::Visuals::ParticleTracer);
			SAVE_VAR(Vars::Glow::Main::Wireframe);
			SAVE_VAR(Vars::Visuals::Vision);
			SAVE_VAR(Vars::ESP::Players::WeaponIcon);


		}

		//Misc
		{
			SAVE_VAR(Vars::Misc::AutoJump);
			SAVE_VAR(Vars::Misc::TauntSlide);
			SAVE_VAR(Vars::Misc::TauntControl);
			SAVE_VAR(Vars::Misc::BypassPure);
			SAVE_VAR(Vars::Misc::NoisemakerSpam);
			SAVE_VAR(Vars::Misc::DisableInterpolation);
			SAVE_VAR(Vars::Misc::MedalFlip);
			SAVE_VAR(Vars::Misc::AutoRocketJump);
			SAVE_VAR(Vars::Misc::ChatSpam);
			SAVE_VAR(Vars::Misc::NoPush);
			SAVE_VAR(Vars::Misc::AutoStrafe);
			SAVE_VAR(Vars::Misc::Directional);
			SAVE_VAR(Vars::Misc::EdgeJump);
			SAVE_VAR(Vars::Misc::EdgeJumpKey);
			SAVE_VAR(Vars::Misc::AntiAFK);
			SAVE_VAR(Vars::Misc::CheatsBypass);
			SAVE_VAR(Vars::Misc::ChatCensor);
			SAVE_VAR(Vars::Misc::RageRetry);
			SAVE_VAR(Vars::Misc::RageRetryHealth);
			SAVE_VAR(Vars::Misc::MVMRes);
			SAVE_VAR(Vars::Misc::BeCat);
			SAVE_VAR(Vars::Misc::VoteRevealer);
			SAVE_VAR(Vars::Misc::VotesInChat);
			// CL_Move
			{
				SAVE_VAR(Vars::Misc::CL_Move::Enabled);//Enabled
				SAVE_VAR(Vars::Misc::CL_Move::Doubletap);// { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::WaitForDT);// { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::NotInAir);// { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::DoubletapKey);// { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::TeleportKey);// { 0x46, L"Teleport Key" }; //F
				SAVE_VAR(Vars::Misc::CL_Move::RechargeKey);// { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::RechargeWhileDead);
				SAVE_VAR(Vars::Misc::CL_Move::AutoRecharge);
				SAVE_VAR(Vars::Misc::CL_Move::AntiWarp);
				SAVE_VAR(Vars::Misc::CL_Move::DTBarStyle);
				SAVE_VAR(Vars::Misc::CL_Move::DTMode);
				SAVE_VAR(Vars::Misc::CL_Move::DtbarOutlineHeight);
				SAVE_VAR(Vars::Misc::CL_Move::DtbarOutlineWidth);
				SAVE_VAR(Vars::Misc::CL_Move::DTBarScaleX);
				SAVE_VAR(Vars::Misc::CL_Move::DTBarScaleY);
				SAVE_VAR(Vars::Misc::CL_Move::DTBarX);
				SAVE_VAR(Vars::Misc::CL_Move::DTBarY);
				SAVE_VAR(Vars::Misc::CL_Move::Fakelag);// { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::FakelagIndicator);
				SAVE_VAR(Vars::Misc::CL_Move::FakelagMin);
				SAVE_VAR(Vars::Misc::CL_Move::FakelagMax);
				SAVE_VAR(Vars::Misc::CL_Move::FakelagMode);
				SAVE_VAR(Vars::Misc::CL_Move::FakelagOnKey);// { 0x52, L"Recharge Key" }; //
				SAVE_VAR(Vars::Misc::CL_Move::FakelagKey);// { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::FakelagValue);// { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::DTTicks);
			}
			//Discord
			{
				SAVE_VAR(Vars::Misc::Discord::EnableRPC);
				SAVE_VAR(Vars::Misc::Discord::IncludeMap);
				SAVE_VAR(Vars::Misc::Discord::IncludeClass);
				SAVE_VAR(Vars::Misc::Discord::IncludeTimestamp);
				SAVE_VAR(Vars::Misc::Discord::WhatImagesShouldBeUsed);
			}
		}

		//AntiHack
		{
			//AntiAim
			{
				SAVE_VAR(Vars::AntiHack::AntiAim::Active);
				SAVE_VAR(Vars::AntiHack::AntiAim::Pitch);
				SAVE_VAR(Vars::AntiHack::AntiAim::YawReal);
				SAVE_VAR(Vars::AntiHack::AntiAim::YawFake);
				SAVE_VAR(Vars::AntiHack::AntiAim::SpinSpeed);
			}
			//Resolver
			{
				SAVE_VAR(Vars::AntiHack::Resolver::Resolver);
			}
		}

		//Others
		{
			SAVE_OTHER(Vars::Menu::Colors::WindowBackground);
			SAVE_OTHER(Vars::Menu::Colors::TitleBar);
			SAVE_OTHER(Vars::Menu::Colors::Text);
			SAVE_OTHER(Vars::Menu::Colors::Widget);
			SAVE_OTHER(Vars::Menu::Colors::WidgetActive);
			SAVE_OTHER(Vars::Menu::Colors::OutlineMenu);
			SAVE_OTHER(Vars::Menu::Colors::FeatureOn);
			SAVE_OTHER(Vars::Menu::Colors::FeatureOff);
			SAVE_OTHER(Vars::Menu::Colors::FeatureBackground);
			SAVE_OTHER(Vars::Menu::Colors::FeatureOutline);

			SAVE_OTHER(Colors::OutlineESP);
			SAVE_OTHER(Colors::Cond);
			SAVE_OTHER(Colors::Target);
			SAVE_OTHER(Colors::Invuln);
			SAVE_OTHER(Colors::Cloak);
			SAVE_OTHER(Colors::Friend);
			SAVE_OTHER(Colors::Local);
			SAVE_OTHER(Colors::Ignored);
			SAVE_OTHER(Colors::Overheal);
			SAVE_OTHER(Colors::Health);
			SAVE_OTHER(Colors::Ammo);
			SAVE_OTHER(Colors::UberColor);
			SAVE_OTHER(Colors::TeamRed);
			SAVE_OTHER(Colors::TeamBlu);
			SAVE_OTHER(Colors::Enemy);
			SAVE_OTHER(Colors::Team);
			SAVE_OTHER(Colors::Hands);
			SAVE_OTHER(Colors::Weapon);
			SAVE_OTHER(Colors::HandsOverlay);
			SAVE_OTHER(Colors::WeaponOverlay);
			SAVE_OTHER(Colors::WorldModulation);
			SAVE_OTHER(Colors::SkyModulation);
			SAVE_OTHER(Colors::StaticPropModulation);
			SAVE_OTHER(Colors::FOVCircle);
			SAVE_OTHER(Colors::Bones);
			SAVE_OTHER(Colors::BulletTracer);
			SAVE_OTHER(Colors::FresnelBase);
			SAVE_OTHER(Colors::FresnelTop);
			SAVE_OTHER(Colors::AimSquareCol);
			SAVE_OTHER(Colors::DtChargingLeft);
			SAVE_OTHER(Colors::DtChargingRight);
			SAVE_OTHER(Colors::DtChargedLeft);
			SAVE_OTHER(Colors::DtChargedRight);
			SAVE_OTHER(Colors::DtOutline);
			SAVE_OTHER(Colors::DmgLoggerBackground);
			SAVE_OTHER(Colors::DmgLoggerOutline);
			SAVE_OTHER(Colors::DmgLoggerText);
			SAVE_OTHER(Colors::WeaponIcon);
			SAVE_OTHER(Colors::NoscopeLines1);
			SAVE_OTHER(Colors::NoscopeLines2);
			SAVE_OTHER(Colors::bonecolor);

			SAVE_OTHER(g_Radar.m_nRadarX);
			SAVE_OTHER(g_Radar.m_nRadarY);
			SAVE_OTHER(Vars::Skybox::SkyboxNum);
			SAVE_STRING(Vars::Skybox::SkyboxName);
			SAVE_OTHER(g_SpectatorList.m_nSpecListX);
			SAVE_OTHER(g_SpectatorList.m_nSpecListY);
			SAVE_OTHER(g_Keybinds.m_nKeybindsX);
			SAVE_OTHER(g_Keybinds.m_nKeybindsY);

		}

		//Fonts
		{
			//FONT_ESP
			{
				SAVE_STRING(Vars::Fonts::FONT_ESP::szName);
				SAVE_VAR(Vars::Fonts::FONT_ESP::nTall);
				SAVE_VAR(Vars::Fonts::FONT_ESP::nWeight);
				SAVE_VAR(Vars::Fonts::FONT_ESP::nFlags);
			}
			//FONT_ESP_NAME
			{
				SAVE_STRING(Vars::Fonts::FONT_ESP_NAME::szName);
				SAVE_VAR(Vars::Fonts::FONT_ESP_NAME::nTall);
				SAVE_VAR(Vars::Fonts::FONT_ESP_NAME::nWeight);
				SAVE_VAR(Vars::Fonts::FONT_ESP_NAME::nFlags);
			}
			//FONT_ESP_COND
			{
				SAVE_STRING(Vars::Fonts::FONT_ESP_COND::szName);
				SAVE_VAR(Vars::Fonts::FONT_ESP_COND::nTall);
				SAVE_VAR(Vars::Fonts::FONT_ESP_COND::nWeight);
				SAVE_VAR(Vars::Fonts::FONT_ESP_COND::nFlags);
			}
			//FONT_ESP_PICKUPS
			{
				SAVE_STRING(Vars::Fonts::FONT_ESP_PICKUPS::szName);
				SAVE_VAR(Vars::Fonts::FONT_ESP_PICKUPS::nTall);
				SAVE_VAR(Vars::Fonts::FONT_ESP_PICKUPS::nWeight);
				SAVE_VAR(Vars::Fonts::FONT_ESP_PICKUPS::nFlags);
			}
			//FONT_MENU
			{
				SAVE_STRING(Vars::Fonts::FONT_MENU::szName);
				SAVE_VAR(Vars::Fonts::FONT_MENU::nTall);
				SAVE_VAR(Vars::Fonts::FONT_MENU::nWeight);
				SAVE_VAR(Vars::Fonts::FONT_MENU::nFlags);
			}
			//FONT_INDICATORS
			{
				SAVE_STRING(Vars::Fonts::FONT_INDICATORS::szName);
				SAVE_VAR(Vars::Fonts::FONT_INDICATORS::nTall);
				SAVE_VAR(Vars::Fonts::FONT_INDICATORS::nWeight);
				SAVE_VAR(Vars::Fonts::FONT_INDICATORS::nFlags);
			}
		}

		m_Write.close();
	}
}

void CConfigManager::Load(const wchar_t *name)
{
	m_Read = std::wifstream(m_sConfigPath + L"\\" + name + _(L".fed"));

	if (m_Read.is_open())
	{
		//Aimbot
		{
			//Global
			{
				LOAD_VAR(Vars::Aimbot::Global::Active);
				LOAD_VAR(Vars::Aimbot::Global::AimKey);
				LOAD_VAR(Vars::Aimbot::Global::AimFOV);
				LOAD_VAR(Vars::Aimbot::Global::AutoShoot);
				LOAD_VAR(Vars::Aimbot::Global::AimPlayers);
				LOAD_VAR(Vars::Aimbot::Global::AimBuildings);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreInvlunerable);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreCloaked);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreFriends);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreTaunting);
				LOAD_VAR(Vars::Aimbot::Global::BAimLethal);
				LOAD_VAR(Vars::Aimbot::Global::showHitboxes);
				LOAD_VAR(Vars::Aimbot::Global::clearPreviousHitbox);
				LOAD_VAR(Vars::Aimbot::Global::hitboxTime);
			}

			{
				LOAD_VAR(Vars::Crits::Active);
				LOAD_VAR(Vars::Crits::CritKey);
			}

			//Hitscan
			{
				//LOAD_VAR(Vars::Aimbot::Hitscan::Active);
				LOAD_VAR(Vars::Aimbot::Hitscan::SortMethod);
				LOAD_VAR(Vars::Aimbot::Hitscan::AimMethod);
				LOAD_VAR(Vars::Aimbot::Hitscan::AimHitbox);
				//LOAD_VAR(Vars::Aimbot::Hitscan::AimFOV);
				LOAD_VAR(Vars::Aimbot::Hitscan::SmoothingAmount);
				LOAD_VAR(Vars::Aimbot::Hitscan::TapFire);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScanHitboxes);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScanHead);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScanBuildings);
				LOAD_VAR(Vars::Aimbot::Hitscan::WaitForHeadshot);
				LOAD_VAR(Vars::Aimbot::Hitscan::WaitForCharge);
				LOAD_VAR(Vars::Aimbot::Hitscan::SpectatedSmooth);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScopedOnly);
				LOAD_VAR(Vars::Aimbot::Hitscan::AutoScope);
				LOAD_VAR(Vars::Aimbot::Hitscan::AutoRev);
				
			}

			//Projectile
			{
				//LOAD_VAR(Vars::Aimbot::Projectile::Active);
				LOAD_VAR(Vars::Aimbot::Projectile::PerformanceMode);
				LOAD_VAR(Vars::Aimbot::Projectile::SortMethod);
				LOAD_VAR(Vars::Aimbot::Projectile::AimMethod);
				LOAD_VAR(Vars::Aimbot::Projectile::AimPosition);
				LOAD_VAR(Vars::Aimbot::Projectile::FeetAimIfOnGround);
				LOAD_VAR(Vars::Aimbot::Projectile::ManualZAdjust);
				LOAD_VAR(Vars::Aimbot::Projectile::ZAdjustAmount);
				//LOAD_VAR(Vars::Aimbot::Projectile::AimFOV);
			}

			//Melee
			{
				//LOAD_VAR(Vars::Aimbot::Melee::Active);
				LOAD_VAR(Vars::Aimbot::Melee::SortMethod);
				LOAD_VAR(Vars::Aimbot::Melee::AimMethod);
				//LOAD_VAR(Vars::Aimbot::Melee::AimFOV);
				LOAD_VAR(Vars::Aimbot::Melee::SmoothingAmount);
				LOAD_VAR(Vars::Aimbot::Melee::RangeCheck);
				LOAD_VAR(Vars::Aimbot::Melee::PredictSwing);
			}
		}

		//Triggerbot
		{
			//Global
			{
				LOAD_VAR(Vars::Triggerbot::Global::Active);
				LOAD_VAR(Vars::Triggerbot::Global::TriggerKey);
				LOAD_VAR(Vars::Triggerbot::Global::IgnoreInvlunerable);
				LOAD_VAR(Vars::Triggerbot::Global::IgnoreCloaked);
				LOAD_VAR(Vars::Triggerbot::Global::IgnoreFriends);
			}

			//Shoot
			{
				LOAD_VAR(Vars::Triggerbot::Shoot::Active);
				LOAD_VAR(Vars::Triggerbot::Shoot::TriggerPlayers);
				LOAD_VAR(Vars::Triggerbot::Shoot::TriggerBuildings);
				LOAD_VAR(Vars::Triggerbot::Shoot::HeadOnly);
				LOAD_VAR(Vars::Triggerbot::Shoot::WaitForCharge);
				LOAD_VAR(Vars::Triggerbot::Shoot::HeadScale);
			}

			//Stab
			{
				LOAD_VAR(Vars::Triggerbot::Stab::Active);
				LOAD_VAR(Vars::Triggerbot::Stab::RageMode);
				LOAD_VAR(Vars::Triggerbot::Stab::Silent);
				LOAD_VAR(Vars::Triggerbot::Stab::Disguise);
				LOAD_VAR(Vars::Triggerbot::Stab::IgnRazor);
				LOAD_VAR(Vars::Triggerbot::Stab::Range);
			}

			//Detonate
			{
				LOAD_VAR(Vars::Triggerbot::Detonate::Active);
				LOAD_VAR(Vars::Triggerbot::Detonate::Stickies);
				LOAD_VAR(Vars::Triggerbot::Detonate::Flares);
				LOAD_VAR(Vars::Triggerbot::Detonate::RadiusScale);
			}

			//Blast
			{
				LOAD_VAR(Vars::Triggerbot::Blast::Active);
				LOAD_VAR(Vars::Triggerbot::Blast::Rage);
				LOAD_VAR(Vars::Triggerbot::Blast::Silent);
				LOAD_VAR(Vars::Triggerbot::Blast::Fov);
			}

			//Uber
			{
				LOAD_VAR(Vars::Triggerbot::Uber::Active);
				LOAD_VAR(Vars::Triggerbot::Uber::OnlyFriends);
				LOAD_VAR(Vars::Triggerbot::Uber::PopLocal);
				LOAD_VAR(Vars::Triggerbot::Uber::AutoVacc);
				LOAD_VAR(Vars::Triggerbot::Uber::HealthLeft);
			}
		}

		//ESP
		{
			//Main
			{
				LOAD_VAR(Vars::ESP::Main::Active);
				LOAD_VAR(Vars::ESP::Main::Outlinedbar);
				LOAD_VAR(Vars::ESP::Main::EnableTeamEnemyColors);
			}

			//Players
			{
				LOAD_VAR(Vars::ESP::Players::Active);
				LOAD_VAR(Vars::ESP::Players::ShowLocal);
				LOAD_VAR(Vars::ESP::Players::IgnoreTeammates);
				LOAD_VAR(Vars::ESP::Players::IgnoreCloaked);
				LOAD_VAR(Vars::ESP::Players::Name);
				LOAD_VAR(Vars::ESP::Players::NameBox);
				LOAD_VAR(Vars::ESP::Players::Uber);
				LOAD_VAR(Vars::ESP::Players::Class);
				LOAD_VAR(Vars::ESP::Players::Health);
				LOAD_VAR(Vars::ESP::Players::Cond);
				LOAD_VAR(Vars::ESP::Players::HealthBar);
				LOAD_VAR(Vars::ESP::Players::Box);
				LOAD_VAR(Vars::ESP::Players::GUID);
				LOAD_VAR(Vars::ESP::Players::Alpha);
				LOAD_VAR(Vars::ESP::Players::Lines);
				LOAD_VAR(Vars::ESP::Players::Bones);
				LOAD_VAR(Vars::ESP::Players::Dlights);
				LOAD_VAR(Vars::ESP::Players::DlightRadius);
				LOAD_VAR(Vars::ESP::Players::WeaponIcon);
			}

			//Buildings
			{
				LOAD_VAR(Vars::ESP::Buildings::Active);
				LOAD_VAR(Vars::ESP::Buildings::IgnoreTeammates);
				LOAD_VAR(Vars::ESP::Buildings::Name);
				LOAD_VAR(Vars::ESP::Buildings::NameBox);
				LOAD_VAR(Vars::ESP::Buildings::Health);
				LOAD_VAR(Vars::ESP::Buildings::Owner);
				LOAD_VAR(Vars::ESP::Buildings::Level);
				LOAD_VAR(Vars::ESP::Buildings::Cond);
				LOAD_VAR(Vars::ESP::Buildings::HealthBar);
				LOAD_VAR(Vars::ESP::Buildings::Box);
				LOAD_VAR(Vars::ESP::Buildings::Alpha);
				LOAD_VAR(Vars::ESP::Buildings::Dlights);
				LOAD_VAR(Vars::ESP::Buildings::DlightRadius);
			}

			//World
			{
				LOAD_VAR(Vars::ESP::World::Active);
				LOAD_VAR(Vars::ESP::World::HealthText);
				LOAD_VAR(Vars::ESP::World::AmmoText);
				LOAD_VAR(Vars::ESP::World::Alpha);
			}
		}

		//Chams
		{
			//Main
			{
				LOAD_VAR(Vars::Chams::Main::Active);
			}

			//Players
			{
				LOAD_VAR(Vars::Chams::Players::Active);
				LOAD_VAR(Vars::Chams::Players::ShowLocal);
				LOAD_VAR(Vars::Chams::Players::IgnoreTeammates);
				LOAD_VAR(Vars::Chams::Players::Wearables);
				LOAD_VAR(Vars::Chams::Players::Weapons);
				LOAD_VAR(Vars::Chams::Players::Material);
				LOAD_VAR(Vars::Chams::Players::IgnoreZ);
				LOAD_VAR(Vars::Chams::Players::Alpha);
			}

			//Buildings
			{
				LOAD_VAR(Vars::Chams::Buildings::Active);
				LOAD_VAR(Vars::Chams::Buildings::IgnoreTeammates);
				LOAD_VAR(Vars::Chams::Buildings::Material);
				LOAD_VAR(Vars::Chams::Buildings::IgnoreZ);
				LOAD_VAR(Vars::Chams::Buildings::Alpha);
			}

			//World
			{
				LOAD_VAR(Vars::Chams::World::Active);
				LOAD_VAR(Vars::Chams::World::Health);
				LOAD_VAR(Vars::Chams::World::Ammo);
				LOAD_VAR(Vars::Chams::World::Projectiles);
				LOAD_VAR(Vars::Chams::World::Material);
				LOAD_VAR(Vars::Chams::World::IgnoreZ);
				LOAD_VAR(Vars::Chams::World::Alpha);
			}

			//DME
			{
				LOAD_VAR(Vars::Chams::DME::Active);
				LOAD_VAR(Vars::Chams::DME::Hands);
				LOAD_VAR(Vars::Chams::DME::HandsAlpha);
				LOAD_VAR(Vars::Chams::DME::HandsGlowOverlay);
				LOAD_VAR(Vars::Chams::DME::Weapon);
				LOAD_VAR(Vars::Chams::DME::WeaponAlpha);
				LOAD_VAR(Vars::Chams::DME::WeaponGlowOverlay);
				LOAD_VAR(Vars::Chams::DME::WeaponRimMultiplier);
				LOAD_VAR(Vars::Chams::DME::HandsRimMultiplier);
			}
		}

		//Glow
		{
			//Main
			{
				LOAD_VAR(Vars::Glow::Main::Active);
				LOAD_VAR(Vars::Glow::Main::Scale);
			}

			//Players
			{
				LOAD_VAR(Vars::Glow::Players::Active);
				LOAD_VAR(Vars::Glow::Players::ShowLocal);
				LOAD_VAR(Vars::Glow::Players::LocalRainbow);
				LOAD_VAR(Vars::Glow::Players::IgnoreTeammates);
				LOAD_VAR(Vars::Glow::Players::Wearables);
				LOAD_VAR(Vars::Glow::Players::Weapons);
				LOAD_VAR(Vars::Glow::Players::Alpha);
				LOAD_VAR(Vars::Glow::Players::Color);
			}

			//Buildings
			{
				LOAD_VAR(Vars::Glow::Buildings::Active);
				LOAD_VAR(Vars::Glow::Buildings::IgnoreTeammates);
				LOAD_VAR(Vars::Glow::Buildings::Alpha);
				LOAD_VAR(Vars::Glow::Buildings::Color);
			}

			//World
			{
				LOAD_VAR(Vars::Glow::World::Active);
				LOAD_VAR(Vars::Glow::World::Health);
				LOAD_VAR(Vars::Glow::World::Ammo);
				LOAD_VAR(Vars::Glow::World::Projectiles);
				LOAD_VAR(Vars::Glow::World::Alpha);
			}
		}

		//Radar
		{
			//Main
			{
				LOAD_VAR(Vars::Radar::Main::Active);
				LOAD_VAR(Vars::Radar::Main::BackAlpha);
				LOAD_VAR(Vars::Radar::Main::LineAlpha);
				LOAD_VAR(Vars::Radar::Main::Size);
				LOAD_VAR(Vars::Radar::Main::Range);
			}

			//Players
			{
				LOAD_VAR(Vars::Radar::Players::Active);
				LOAD_VAR(Vars::Radar::Players::IconType);
				LOAD_VAR(Vars::Radar::Players::BackGroundType);
				LOAD_VAR(Vars::Radar::Players::Outline);
				LOAD_VAR(Vars::Radar::Players::IgnoreTeam);
				LOAD_VAR(Vars::Radar::Players::IgnoreCloaked);
				LOAD_VAR(Vars::Radar::Players::Health);
			}

			//Buildings
			{
				LOAD_VAR(Vars::Radar::Buildings::Active);
				LOAD_VAR(Vars::Radar::Buildings::Outline);
				LOAD_VAR(Vars::Radar::Buildings::IgnoreTeam);
				LOAD_VAR(Vars::Radar::Buildings::Health);
			}

			//World
			{
				LOAD_VAR(Vars::Radar::World::Active);
				LOAD_VAR(Vars::Radar::World::Health);
				LOAD_VAR(Vars::Radar::World::Ammo);
			}
		}

		//Visuals
		{
			LOAD_VAR(Vars::Visuals::RemoveDisguises);
			LOAD_VAR(Vars::Visuals::RemoveTaunts);
			LOAD_VAR(Vars::Visuals::FieldOfView);
			LOAD_VAR(Vars::Visuals::AimFOVAlpha);
			LOAD_VAR(Vars::Visuals::RemoveScope);
			LOAD_VAR(Vars::Visuals::ScopeLines);
			LOAD_VAR(Vars::Visuals::RemoveZoom);
			LOAD_VAR(Vars::Visuals::RemovePunch);
			LOAD_VAR(Vars::Visuals::CrosshairAimPos);
			LOAD_VAR(Vars::Visuals::ChatInfo);
			LOAD_VAR(Vars::Visuals::OutOfFOVArrowsOutline);
			LOAD_VAR(Vars::Visuals::SpectatorList);

			LOAD_VAR(Vars::Visuals::SpyWarning);
			LOAD_VAR(Vars::Visuals::SpyWarningAnnounce);
			LOAD_VAR(Vars::Visuals::SpyWarningStyle);
			LOAD_VAR(Vars::Visuals::SpyWarningVisibleOnly);
			LOAD_VAR(Vars::Visuals::SpyWarningIgnoreFriends);

			LOAD_VAR(Vars::Visuals::Snow);
			LOAD_VAR(Vars::Visuals::ToolTips);

			LOAD_VAR(Vars::Visuals::ThirdPerson);
			LOAD_VAR(Vars::Visuals::ThirdPersonKey);
			LOAD_VAR(Vars::Visuals::ThirdPersonSilentAngles);
			LOAD_VAR(Vars::Visuals::ThirdPersonInstantYaw);
			LOAD_VAR(Vars::Visuals::ThirdPersonServerHitbox);

			LOAD_VAR(Vars::Visuals::WorldModulation);
			LOAD_VAR(Vars::Visuals::PropWireframe);
			LOAD_VAR(Vars::Visuals::SkyboxChanger);
			LOAD_VAR(Vars::Visuals::ParticleTracer);
			LOAD_VAR(Vars::Visuals::BulletTracer);
			LOAD_VAR(Vars::Visuals::BulletTracerRainbow);
			LOAD_VAR(Vars::Visuals::AimbotViewmodel);
			LOAD_VAR(Vars::Visuals::VMOffX);
			LOAD_VAR(Vars::Visuals::VMOffY);
			LOAD_VAR(Vars::Visuals::VMOffZ);
			LOAD_VAR(Vars::Visuals::VMRoll);
			LOAD_VAR(Vars::Visuals::OutOfFOVArrows);
			LOAD_VAR(Vars::Visuals::ArrowLength);
			LOAD_VAR(Vars::Visuals::ArrowAngle);
			LOAD_VAR(Vars::Visuals::MaxDist);
			LOAD_VAR(Vars::Visuals::MinDist);
			LOAD_VAR(Vars::Visuals::AimPosSquare);
			LOAD_VAR(Vars::Visuals::despawnTime);
			LOAD_VAR(Vars::Visuals::damageLogger);
			LOAD_VAR(Vars::Glow::Main::Wireframe);
			LOAD_VAR(Vars::Visuals::Vision);
			
		}

		//Misc
		{
			LOAD_VAR(Vars::Misc::AutoJump);
			LOAD_VAR(Vars::Misc::TauntSlide);
			LOAD_VAR(Vars::Misc::TauntControl);
			LOAD_VAR(Vars::Misc::BypassPure);
			LOAD_VAR(Vars::Misc::NoisemakerSpam);
			LOAD_VAR(Vars::Misc::DisableInterpolation);
			LOAD_VAR(Vars::Misc::MedalFlip);
			LOAD_VAR(Vars::Misc::AutoRocketJump);
			LOAD_VAR(Vars::Misc::ChatSpam);
			LOAD_VAR(Vars::Misc::NoPush);
			LOAD_VAR(Vars::Misc::EdgeJump);
			LOAD_VAR(Vars::Misc::EdgeJumpKey);
			LOAD_VAR(Vars::Misc::AutoStrafe);
			LOAD_VAR(Vars::Misc::Directional);
			LOAD_VAR(Vars::Misc::AntiAFK);
			LOAD_VAR(Vars::Misc::CheatsBypass);
			LOAD_VAR(Vars::Misc::ChatCensor);
			LOAD_VAR(Vars::Misc::RageRetry);
			LOAD_VAR(Vars::Misc::RageRetryHealth);
			LOAD_VAR(Vars::Misc::MVMRes);
			LOAD_VAR(Vars::Misc::BeCat);
			LOAD_VAR(Vars::Misc::VoteRevealer);
			LOAD_VAR(Vars::Misc::VotesInChat);
			// CL_Move
			{
				LOAD_VAR(Vars::Misc::CL_Move::Enabled);//Enabled
				LOAD_VAR(Vars::Misc::CL_Move::Doubletap);// { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::WaitForDT);// { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::NotInAir);// { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::TeleportKey);// { 0x46, L"Teleport Key" }; //F
				LOAD_VAR(Vars::Misc::CL_Move::RechargeKey);// { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::RechargeWhileDead);
				LOAD_VAR(Vars::Misc::CL_Move::AutoRecharge);
				LOAD_VAR(Vars::Misc::CL_Move::AntiWarp);
				LOAD_VAR(Vars::Misc::CL_Move::DoubletapKey);// { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::DTBarStyle);
				LOAD_VAR(Vars::Misc::CL_Move::DTMode);
				LOAD_VAR(Vars::Misc::CL_Move::DtbarOutlineHeight);
				LOAD_VAR(Vars::Misc::CL_Move::DtbarOutlineWidth);
				LOAD_VAR(Vars::Misc::CL_Move::DTBarScaleX);
				LOAD_VAR(Vars::Misc::CL_Move::DTBarScaleY);
				LOAD_VAR(Vars::Misc::CL_Move::DTBarX);
				LOAD_VAR(Vars::Misc::CL_Move::DTBarY);
				LOAD_VAR(Vars::Misc::CL_Move::Fakelag);// { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::FakelagIndicator);
				LOAD_VAR(Vars::Misc::CL_Move::FakelagMin);
				LOAD_VAR(Vars::Misc::CL_Move::FakelagMax);
				LOAD_VAR(Vars::Misc::CL_Move::FakelagMode);
				LOAD_VAR(Vars::Misc::CL_Move::FakelagOnKey);// { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::FakelagKey);// { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::FakelagValue);// { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::DTTicks);
			}
			//Discord
			{
				LOAD_VAR(Vars::Misc::Discord::EnableRPC);
				LOAD_VAR(Vars::Misc::Discord::IncludeMap);
				LOAD_VAR(Vars::Misc::Discord::IncludeClass);
				LOAD_VAR(Vars::Misc::Discord::IncludeTimestamp);
				LOAD_VAR(Vars::Misc::Discord::WhatImagesShouldBeUsed);
			}
		}

		//AntiHack
		{
			//AntiAim
			{
				LOAD_VAR(Vars::AntiHack::AntiAim::Active);
				LOAD_VAR(Vars::AntiHack::AntiAim::Pitch);
				LOAD_VAR(Vars::AntiHack::AntiAim::YawReal);
				LOAD_VAR(Vars::AntiHack::AntiAim::YawFake);
				LOAD_VAR(Vars::AntiHack::AntiAim::SpinSpeed);
			}
			//Resolver
			{
				LOAD_VAR(Vars::AntiHack::Resolver::Resolver);
			}
		}

		//Others
		{
			LOAD_OTHER(Vars::Menu::Colors::WindowBackground);
			LOAD_OTHER(Vars::Menu::Colors::TitleBar);
			LOAD_OTHER(Vars::Menu::Colors::Text);
			LOAD_OTHER(Vars::Menu::Colors::Widget);
			LOAD_OTHER(Vars::Menu::Colors::WidgetActive);
			LOAD_OTHER(Vars::Menu::Colors::OutlineMenu);
			LOAD_OTHER(Vars::Menu::Colors::FeatureOn);
			LOAD_OTHER(Vars::Menu::Colors::FeatureOff);
			LOAD_OTHER(Vars::Menu::Colors::FeatureBackground);
			LOAD_OTHER(Vars::Menu::Colors::FeatureOutline);


			LOAD_OTHER(Colors::OutlineESP);
			LOAD_OTHER(Colors::Cond);
			LOAD_OTHER(Colors::Target);
			LOAD_OTHER(Colors::Invuln);
			LOAD_OTHER(Colors::Cloak);
			LOAD_OTHER(Colors::Friend);
			LOAD_OTHER(Colors::Local);
			LOAD_OTHER(Colors::Ignored);
			LOAD_OTHER(Colors::Overheal);
			LOAD_OTHER(Colors::Health);
			LOAD_OTHER(Colors::Ammo);
			LOAD_OTHER(Colors::UberColor);
			LOAD_OTHER(Colors::TeamRed);
			LOAD_OTHER(Colors::TeamBlu);
			LOAD_OTHER(Colors::Enemy);
			LOAD_OTHER(Colors::Team);
			LOAD_OTHER(Colors::Hands);
			LOAD_OTHER(Colors::HandsOverlay);
			LOAD_OTHER(Colors::Weapon);
			LOAD_OTHER(Colors::WeaponOverlay);
			LOAD_OTHER(Colors::WorldModulation);
			LOAD_OTHER(Colors::SkyModulation);
			LOAD_OTHER(Colors::StaticPropModulation);
			LOAD_OTHER(Colors::FOVCircle);
			LOAD_OTHER(Colors::Bones);
			LOAD_OTHER(Colors::BulletTracer);
			LOAD_OTHER(Colors::FresnelBase);
			LOAD_OTHER(Colors::FresnelTop);
			LOAD_OTHER(Colors::AimSquareCol);
			LOAD_OTHER(Colors::DtChargingLeft);
			LOAD_OTHER(Colors::DtChargingRight);
			LOAD_OTHER(Colors::DtChargedLeft);
			LOAD_OTHER(Colors::DtChargedRight);
			LOAD_OTHER(Colors::DtOutline);
			LOAD_OTHER(Colors::DmgLoggerBackground);
			LOAD_OTHER(Colors::DmgLoggerOutline);
			LOAD_OTHER(Colors::DmgLoggerText);
			LOAD_OTHER(Colors::WeaponIcon);
			LOAD_OTHER(Colors::NoscopeLines1);
			LOAD_OTHER(Colors::NoscopeLines2);
			LOAD_OTHER(Colors::bonecolor);

			LOAD_OTHER(g_Radar.m_nRadarX);
			LOAD_OTHER(g_Radar.m_nRadarY);

			LOAD_OTHER(g_SpectatorList.m_nSpecListX);
			LOAD_OTHER(g_SpectatorList.m_nSpecListY);

			LOAD_OTHER(Vars::Skybox::SkyboxNum);
			LOAD_STRING(Vars::Skybox::SkyboxName);
			LOAD_OTHER(g_Keybinds.m_nKeybindsX);
			LOAD_OTHER(g_Keybinds.m_nKeybindsY);
		}

		//Fonts
		{
			//FONT_ESP
			{
				LOAD_STRING(Vars::Fonts::FONT_ESP::szName);
				LOAD_VAR(Vars::Fonts::FONT_ESP::nTall);
				LOAD_VAR(Vars::Fonts::FONT_ESP::nWeight);
				LOAD_VAR(Vars::Fonts::FONT_ESP::nFlags);
			}
			//FONT_ESP_NAME
			{
				LOAD_STRING(Vars::Fonts::FONT_ESP_NAME::szName);
				LOAD_VAR(Vars::Fonts::FONT_ESP_NAME::nTall);
				LOAD_VAR(Vars::Fonts::FONT_ESP_NAME::nWeight);
				LOAD_VAR(Vars::Fonts::FONT_ESP_NAME::nFlags);
			}
			//FONT_ESP_COND
			{
				LOAD_STRING(Vars::Fonts::FONT_ESP_COND::szName);
				LOAD_VAR(Vars::Fonts::FONT_ESP_COND::nTall);
				LOAD_VAR(Vars::Fonts::FONT_ESP_COND::nWeight);
				LOAD_VAR(Vars::Fonts::FONT_ESP_COND::nFlags);
			}
			//FONT_ESP_PICKUPS
			{
				LOAD_STRING(Vars::Fonts::FONT_ESP_PICKUPS::szName);
				LOAD_VAR(Vars::Fonts::FONT_ESP_PICKUPS::nTall);
				LOAD_VAR(Vars::Fonts::FONT_ESP_PICKUPS::nWeight);
				LOAD_VAR(Vars::Fonts::FONT_ESP_PICKUPS::nFlags);
			}
			//FONT_MENU
			{
				LOAD_STRING(Vars::Fonts::FONT_MENU::szName);
				LOAD_VAR(Vars::Fonts::FONT_MENU::nTall);
				LOAD_VAR(Vars::Fonts::FONT_MENU::nWeight);
				LOAD_VAR(Vars::Fonts::FONT_MENU::nFlags);
			}
			//FONT_INDICATORS
			{
				LOAD_STRING(Vars::Fonts::FONT_INDICATORS::szName);
				LOAD_VAR(Vars::Fonts::FONT_INDICATORS::nTall);
				LOAD_VAR(Vars::Fonts::FONT_INDICATORS::nWeight);
				LOAD_VAR(Vars::Fonts::FONT_INDICATORS::nFlags);
			}
		}

		m_Read.close();
	}
}

void CConfigManager::Remove(const wchar_t *name)
{
	std::filesystem::remove(m_sConfigPath + L"\\" + name + _(L".fed"));
}
