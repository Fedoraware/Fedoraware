#include "ConfigManager.h"

#include <string>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../../Vars.h"
#include "../../../SDK/SDK.h"
#include "../../Misc/Notifications/Notifications.h"

#define SAVE_VAR(x) SaveJson(#x, x.Value)
#define LOAD_VAR(x) LoadJson(#x, x.Value)

#define SAVE_STRING(x) SaveJson(#x, x)
#define LOAD_STRING(x) LoadJson(#x, x)

#define SAVE_OTHER(x) SaveJson(#x, x)
#define LOAD_OTHER(x) LoadJson(#x, x)

boost::property_tree::ptree WriteTree;
boost::property_tree::ptree ReadTree;

boost::property_tree::ptree ColorToTree(Color_t color)
{
	boost::property_tree::ptree colorTree;
	colorTree.put("r", color.r);
	colorTree.put("g", color.g);
	colorTree.put("b", color.b);
	colorTree.put("a", color.a);

	return colorTree;
}

Color_t TreeToColor(const boost::property_tree::ptree& tree)
{
	Color_t treeColor;
	if (auto getValue = tree.get_optional<byte>("r")) { treeColor.r = *getValue; }
	if (auto getValue = tree.get_optional<byte>("g")) { treeColor.g = *getValue; }
	if (auto getValue = tree.get_optional<byte>("b")) { treeColor.b = *getValue; }
	if (auto getValue = tree.get_optional<byte>("a")) { treeColor.a = *getValue; }

	return treeColor;
}

boost::property_tree::ptree VecToTree(const Vec3& vec)
{
	boost::property_tree::ptree vecTree;
	vecTree.put("x", vec.x);
	vecTree.put("y", vec.y);
	vecTree.put("z", vec.z);

	return vecTree;
}

Vec3 TreeToVec(const boost::property_tree::ptree& tree)
{
	Vec3 treeVec;
	if (auto getValue = tree.get_optional<float>("x")) { treeVec.x = *getValue; }
	if (auto getValue = tree.get_optional<float>("y")) { treeVec.y = *getValue; }
	if (auto getValue = tree.get_optional<float>("z")) { treeVec.z = *getValue; }

	return treeVec;
}

void CConfigManager::SaveJson(const char* name, bool val)
{
	WriteTree.put(name, val);
}

void CConfigManager::SaveJson(const char* name, const std::string& val)
{
	WriteTree.put(name, val);
}

void CConfigManager::SaveJson(const char* name, int val)
{
	WriteTree.put(name, val);
}

void CConfigManager::SaveJson(const char* name, float val)
{
	WriteTree.put(name, val);
}

void CConfigManager::SaveJson(const char* name, Color_t val)
{
	WriteTree.put_child(name, ColorToTree(val));
}

void CConfigManager::SaveJson(const char* name, Gradient_t val)
{
	boost::property_tree::ptree gradientTree;
	gradientTree.put_child("startColour", ColorToTree(val.startColour));
	gradientTree.put_child("endColour", ColorToTree(val.endColour));

	WriteTree.put_child(name, gradientTree);
}

void CConfigManager::SaveJson(const char* name, const Vec3& val)
{
	WriteTree.put_child(name, VecToTree(val));
}

void CConfigManager::SaveJson(const char* name, const Chams_t& val)
{
	boost::property_tree::ptree chamTree;
	chamTree.put("showObstructed", val.showObstructed);
	chamTree.put("drawMaterial", val.drawMaterial);
	chamTree.put("overlayType", val.overlayType);
	chamTree.put("chamsActive", val.chamsActive);
	chamTree.put("rainbow", val.rainbow);
	chamTree.put("overlayRainbow", val.overlayRainbow);
	chamTree.put("overlayPulse", val.overlayPulse);
	chamTree.put("overlayIntensity", val.overlayIntensity);
	chamTree.put_child("fresnelBase", ColorToTree(val.fresnelBase));
	chamTree.put_child("colour", ColorToTree(val.colour));
	chamTree.put_child("overlayColour", ColorToTree(val.overlayColour));
	chamTree.put("customMaterial", val.customMaterial);

	WriteTree.put_child(name, chamTree);
}

void CConfigManager::SaveJson(const char* name, const DragBox_t& val)
{
	boost::property_tree::ptree dragBoxTree;
	dragBoxTree.put("x", val.x);
	dragBoxTree.put("y", val.y);
	dragBoxTree.put("w", val.w);
	dragBoxTree.put("h", val.h);
	dragBoxTree.put("c", val.c);

	WriteTree.put_child(name, dragBoxTree);
}

void CConfigManager::LoadJson(const char* name, std::string& val)
{
	if (auto getValue = ReadTree.get_optional<std::string>(name))
	{
		val = *getValue;
	}
}

void CConfigManager::LoadJson(const char* name, bool& val)
{
	if (auto getValue = ReadTree.get_optional<bool>(name))
	{
		val = *getValue;
	}
}

void CConfigManager::LoadJson(const char* name, int& val)
{
	if (auto getValue = ReadTree.get_optional<int>(name))
	{
		val = *getValue;
	}
}

void CConfigManager::LoadJson(const char* name, float& val)
{
	if (auto getValue = ReadTree.get_optional<float>(name))
	{
		val = *getValue;
	}
}

void CConfigManager::LoadJson(const char* name, Color_t& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		val = TreeToColor(*getChild);
	}
}

void CConfigManager::LoadJson(const char* name, Gradient_t& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		if (const auto getStartColor = (*getChild).get_child_optional("startColour"))
		{
			val.startColour = TreeToColor(*getStartColor);
		}
		if (const auto endColor = (*getChild).get_child_optional("endColour"))
		{
			val.endColour = TreeToColor(*endColor);
		}
	}
}

void CConfigManager::LoadJson(const char* name, Vec3& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		val = TreeToVec(*getChild);
	}
}

void CConfigManager::LoadJson(const char* name, Chams_t& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		if (auto getValue = (*getChild).get_optional<bool>("showObstructed")) { val.showObstructed = *getValue; }
		if (auto getValue = (*getChild).get_optional<int>("drawMaterial")) { val.drawMaterial = *getValue; }
		if (auto getValue = (*getChild).get_optional<int>("overlayType")) { val.overlayType = *getValue; }
		if (auto getValue = (*getChild).get_optional<bool>("chamsActive")) { val.chamsActive = *getValue; }
		if (auto getValue = (*getChild).get_optional<bool>("rainbow")) { val.rainbow = *getValue; }
		if (auto getValue = (*getChild).get_optional<bool>("overlayRainbow")) { val.overlayRainbow = *getValue; }
		if (auto getValue = (*getChild).get_optional<bool>("overlayPulse")) { val.overlayPulse = *getValue; }
		if (auto getValue = (*getChild).get_optional<float>("overlayIntensity")) { val.overlayIntensity = *getValue; }
		if (const auto getChildColor = (*getChild).get_child_optional("fresnelBase")) { val.fresnelBase = TreeToColor(*getChildColor); }
		if (const auto getChildColor = (*getChild).get_child_optional("colour")) { val.colour = TreeToColor(*getChildColor); }
		if (const auto getChildColor = (*getChild).get_child_optional("overlayColour")) { val.overlayColour = TreeToColor(*getChildColor); }
		if (auto getValue = (*getChild).get_optional<std::string>("customMaterial")) { val.customMaterial = *getValue; }
	}
}

void CConfigManager::LoadJson(const char* name, DragBox_t& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		if (auto getValue = (*getChild).get_optional<int>("x")) { val.x = *getValue; }
		if (auto getValue = (*getChild).get_optional<int>("y")) { val.y = *getValue; }
		if (auto getValue = (*getChild).get_optional<int>("w")) { val.w = *getValue; }
		if (auto getValue = (*getChild).get_optional<int>("h")) { val.h = *getValue; }
		if (auto getValue = (*getChild).get_optional<int>("c")) { val.c = *getValue; }
		val.update = true;
	}
}

CConfigManager::CConfigManager()
{
	ConfigPath = std::filesystem::current_path().string() + "\\FedFigs";

	// Create 'FedFigs' folder
	if (!std::filesystem::exists(ConfigPath))
	{
		std::filesystem::create_directory(ConfigPath);
	}

	// Create 'Core' folder for Attribute-Changer & Playerlist
	if (!std::filesystem::exists(ConfigPath + "\\Core"))
	{
		std::filesystem::create_directory(ConfigPath + "\\Core");
	}

	// Create 'Materials' folder for custom materials
	if (!std::filesystem::exists(ConfigPath + "\\Materials"))
	{
		std::filesystem::create_directory(ConfigPath + "\\Materials");
	}

	// Create 'Lua' folder for Lua scripts
	if (!std::filesystem::exists(ConfigPath + "\\Lua"))
	{
		std::filesystem::create_directory(ConfigPath + "\\Lua");
	}
}

bool CConfigManager::SaveConfig(const std::string& configName)
{
	try
	{
		WriteTree.clear();

		// Menu
		{
			SAVE_OTHER(Vars::Menu::CheatName);
			SAVE_OTHER(Vars::Menu::CheatPrefix);
			SAVE_VAR(Vars::Menu::Vignette);
			SAVE_VAR(Vars::Menu::ShowDVD);
			SAVE_VAR(Vars::Menu::CloseOnUnfocus);
			SAVE_VAR(Vars::Menu::MenuKey);
		}

		//Aimbot
		{
			//Global
			{
				SAVE_VAR(Vars::Aimbot::Global::Active);
				SAVE_VAR(Vars::Aimbot::Global::AimKey);
				SAVE_VAR(Vars::Aimbot::Global::AimFOV);
				SAVE_VAR(Vars::Aimbot::Global::AutoShoot);
				SAVE_VAR(Vars::Aimbot::Global::DontWaitForShot);
				SAVE_VAR(Vars::Aimbot::Global::FlickatEnemies);
				SAVE_VAR(Vars::Aimbot::Global::AimPlayers);
				SAVE_VAR(Vars::Aimbot::Global::AimBuildings);
				SAVE_VAR(Vars::Aimbot::Global::AimStickies);
				SAVE_VAR(Vars::Aimbot::Global::AimNPC);
				SAVE_VAR(Vars::Aimbot::Global::AimBombs);
				SAVE_VAR(Vars::Aimbot::Global::IgnoreOptions);
				SAVE_VAR(Vars::Aimbot::Global::BAimLethal);
				SAVE_VAR(Vars::Aimbot::Global::showHitboxes);
				SAVE_VAR(Vars::Aimbot::Global::ClearPreviousHitbox);
				SAVE_VAR(Vars::Aimbot::Global::HitboxLifetime);
			}

			// Backtrack
			{
				SAVE_VAR(Vars::Backtrack::Enabled);
				SAVE_VAR(Vars::Backtrack::Latency);
				//Bt Chams
				{
					SAVE_VAR(Vars::Backtrack::BtChams::Enabled);
					SAVE_VAR(Vars::Backtrack::BtChams::LastOnly);
					SAVE_VAR(Vars::Backtrack::BtChams::EnemyOnly);
					SAVE_VAR(Vars::Backtrack::BtChams::IgnoreZ);
					SAVE_VAR(Vars::Backtrack::BtChams::Material);
					SAVE_OTHER(Vars::Backtrack::BtChams::BacktrackColor);
				}
			}

			{
				SAVE_VAR(Vars::CritHack::Active);
				SAVE_VAR(Vars::CritHack::Indicators);
				SAVE_OTHER(Vars::CritHack::IndicatorPos);
				SAVE_VAR(Vars::CritHack::AvoidRandom);
				SAVE_VAR(Vars::CritHack::AlwaysMelee);
				SAVE_VAR(Vars::CritHack::CritKey);
				SAVE_VAR(Vars::CritHack::AutoMeleeCrit);
			}

			//Hitscan
			{
				//SAVE_VAR(Vars::Aimbot::Hitscan::Active);
				SAVE_VAR(Vars::Aimbot::Hitscan::SortMethod);
				SAVE_VAR(Vars::Aimbot::Hitscan::BackTrackMethod);
				SAVE_VAR(Vars::Aimbot::Hitscan::RespectFOV);
				SAVE_VAR(Vars::Aimbot::Hitscan::AimMethod);
				SAVE_VAR(Vars::Aimbot::Hitscan::AimHitbox);
				//SAVE_VAR(Vars::Aimbot::Hitscan::AimFOV);
				SAVE_VAR(Vars::Aimbot::Hitscan::SmoothingAmount);
				SAVE_VAR(Vars::Aimbot::Hitscan::TapFire);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScanHitboxes);
				SAVE_VAR(Vars::Aimbot::Hitscan::MultiHitboxes);
				SAVE_VAR(Vars::Aimbot::Hitscan::StaticHitboxes);
				SAVE_VAR(Vars::Aimbot::Hitscan::PointScale);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScanBuildings);
				SAVE_VAR(Vars::Aimbot::Hitscan::WaitForHeadshot);
				SAVE_VAR(Vars::Aimbot::Hitscan::WaitForCharge);
				SAVE_VAR(Vars::Aimbot::Hitscan::SpectatedSmooth);
				SAVE_VAR(Vars::Aimbot::Hitscan::ScopedOnly);
				SAVE_VAR(Vars::Aimbot::Hitscan::AutoScope);
				SAVE_VAR(Vars::Aimbot::Hitscan::AutoRev);
				SAVE_VAR(Vars::Aimbot::Hitscan::ExtinguishTeam);
			}

			//Projectile
			{
				//SAVE_VAR(Vars::Aimbot::Projectile::Active);
				SAVE_VAR(Vars::Aimbot::Projectile::SortMethod);
				SAVE_VAR(Vars::Aimbot::Projectile::RespectFOV);
				SAVE_VAR(Vars::Aimbot::Projectile::AimMethod);
				SAVE_VAR(Vars::Aimbot::Projectile::AimPosition);
				SAVE_VAR(Vars::Aimbot::Projectile::VisTestPoints);
				SAVE_VAR(Vars::Aimbot::Projectile::AllowedHitboxes);
				SAVE_VAR(Vars::Aimbot::Projectile::FeetAimIfOnGround);
				SAVE_VAR(Vars::Aimbot::Projectile::BounceKey);
				SAVE_VAR(Vars::Aimbot::Projectile::SplashPrediction);
				SAVE_OTHER(Vars::Aimbot::Projectile::PredictionColor);
				SAVE_VAR(Vars::Aimbot::Projectile::PredictionTime);
				SAVE_VAR(Vars::Aimbot::Projectile::ScanPoints);
				SAVE_VAR(Vars::Aimbot::Projectile::ScanScale);
				SAVE_VAR(Vars::Aimbot::Projectile::PredictObscured);
				SAVE_VAR(Vars::Aimbot::Projectile::NoSpread);
				SAVE_VAR(Vars::Aimbot::Projectile::ChargeLooseCannon);
				SAVE_VAR(Vars::Aimbot::Projectile::StrafePredictionAir);
				SAVE_VAR(Vars::Aimbot::Projectile::StrafePredictionGround);
				SAVE_VAR(Vars::Aimbot::Projectile::StrafePredictionSamples);
				SAVE_VAR(Vars::Aimbot::Projectile::StrafePredictionMinDifference);
				SAVE_VAR(Vars::Aimbot::Projectile::StrafePredictionMaxDistance);
				SAVE_VAR(Vars::Aimbot::Projectile::MinSplashPredictionDistance);
				SAVE_VAR(Vars::Aimbot::Projectile::MaxSplashPredictionDistance);
				SAVE_VAR(Vars::Aimbot::Projectile::WaitForHit);
				//SAVE_VAR(Vars::Aimbot::Projectile::AimFOV);
			}

			//Melee
			{
				//SAVE_VAR(Vars::Aimbot::Melee::Active);
				SAVE_VAR(Vars::Aimbot::Melee::SortMethod);
				SAVE_VAR(Vars::Aimbot::Melee::RespectFOV);
				SAVE_VAR(Vars::Aimbot::Melee::AimMethod);
				//SAVE_VAR(Vars::Aimbot::Melee::AimFOV);
				SAVE_VAR(Vars::Aimbot::Melee::SmoothingAmount);
				SAVE_VAR(Vars::Aimbot::Melee::RangeCheck);
				SAVE_VAR(Vars::Aimbot::Melee::PredictSwing);
				SAVE_VAR(Vars::Aimbot::Melee::WhipTeam);
			}
		}

		//Triggerbot
		{
			//Global
			{
				SAVE_VAR(Vars::Triggerbot::Global::Active);
				SAVE_VAR(Vars::Triggerbot::Global::TriggerKey);
				SAVE_VAR(Vars::Triggerbot::Global::IgnoreOptions);
			}

			//Shoot
			{
				SAVE_VAR(Vars::Triggerbot::Shoot::Active);
				SAVE_VAR(Vars::Triggerbot::Shoot::TriggerPlayers);
				SAVE_VAR(Vars::Triggerbot::Shoot::TriggerBuildings);
				SAVE_VAR(Vars::Triggerbot::Shoot::HeadOnly);
				SAVE_VAR(Vars::Triggerbot::Shoot::WaitForHeadshot);
				SAVE_VAR(Vars::Triggerbot::Shoot::ScopeOnly);
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
				SAVE_VAR(Vars::Triggerbot::Detonate::DetonateTargets);
				SAVE_VAR(Vars::Triggerbot::Detonate::Stickies);
				SAVE_VAR(Vars::Triggerbot::Detonate::Flares);
				SAVE_VAR(Vars::Triggerbot::Detonate::RadiusScale);
			}

			//Blast
			{
				SAVE_VAR(Vars::Triggerbot::Blast::Active);
				SAVE_VAR(Vars::Triggerbot::Blast::Rage);
				SAVE_VAR(Vars::Triggerbot::Blast::Silent);
				SAVE_VAR(Vars::Triggerbot::Blast::ExtinguishPlayers);
				SAVE_VAR(Vars::Triggerbot::Blast::DisableOnAttack);
				SAVE_VAR(Vars::Triggerbot::Blast::Fov);
			}

			//Uber
			{
				SAVE_VAR(Vars::Triggerbot::Uber::Active);
				SAVE_VAR(Vars::Triggerbot::Uber::OnlyFriends);
				SAVE_VAR(Vars::Triggerbot::Uber::PopLocal);
				SAVE_VAR(Vars::Triggerbot::Uber::AutoVacc);
				SAVE_VAR(Vars::Triggerbot::Uber::HealthLeft);
				SAVE_VAR(Vars::Triggerbot::Uber::VoiceCommand);
				SAVE_VAR(Vars::Triggerbot::Uber::ReactFoV);
				SAVE_VAR(Vars::Triggerbot::Uber::ReactClasses);
				SAVE_VAR(Vars::Triggerbot::Uber::BlastRes);
				SAVE_VAR(Vars::Triggerbot::Uber::BulletRes);
				SAVE_VAR(Vars::Triggerbot::Uber::FireRes);
			}
		}

		//ESP
		{
			//Main
			{
				SAVE_VAR(Vars::ESP::Main::Active);
				SAVE_VAR(Vars::ESP::Main::Outlinedbar);
				SAVE_VAR(Vars::ESP::Main::EnableTeamEnemyColors);
				SAVE_VAR(Vars::ESP::Main::DistanceToAlpha);
				SAVE_VAR(Vars::ESP::Main::DormantSoundESP);
				SAVE_VAR(Vars::ESP::Main::DormantTime);
				SAVE_VAR(Vars::ESP::Main::DormantDist);
				SAVE_VAR(Vars::ESP::Main::NetworkedDist);
			}

			//Players
			{
				SAVE_VAR(Vars::ESP::Players::Active);
				SAVE_VAR(Vars::ESP::Players::ShowLocal);
				SAVE_VAR(Vars::ESP::Players::IgnoreTeammates);
				SAVE_VAR(Vars::ESP::Players::IgnoreCloaked);
				SAVE_VAR(Vars::ESP::Players::Name);
				SAVE_VAR(Vars::ESP::Players::NameCustom);
				SAVE_OTHER(Vars::ESP::Players::NameColor);
				SAVE_VAR(Vars::ESP::Players::NameBox);
				SAVE_VAR(Vars::ESP::Players::Uber);
				SAVE_VAR(Vars::ESP::Players::Class);
				SAVE_VAR(Vars::ESP::Players::HealthText);
				SAVE_VAR(Vars::ESP::Players::Cond);
				SAVE_VAR(Vars::ESP::Players::HealthBar);
				SAVE_VAR(Vars::ESP::Players::HealthBarStyle);
				SAVE_VAR(Vars::ESP::Players::Box);
				SAVE_VAR(Vars::ESP::Players::GUID);
				SAVE_VAR(Vars::ESP::Players::Choked);
				SAVE_VAR(Vars::ESP::Players::Alpha);
				SAVE_VAR(Vars::ESP::Players::Lines);
				SAVE_VAR(Vars::ESP::Players::Bones);
				SAVE_VAR(Vars::ESP::Players::Dlights);
				SAVE_VAR(Vars::ESP::Players::DlightRadius);
				SAVE_VAR(Vars::ESP::Players::CheaterDetection);
				SAVE_VAR(Vars::ESP::Players::WeaponIcon);
				SAVE_VAR(Vars::ESP::Players::WeaponText);
				SAVE_VAR(Vars::ESP::Players::SniperSightlines);
			}

			//Buildings
			{
				SAVE_VAR(Vars::ESP::Buildings::Active);
				SAVE_VAR(Vars::ESP::Buildings::IgnoreTeammates);
				SAVE_VAR(Vars::ESP::Buildings::Name);
				SAVE_VAR(Vars::ESP::Buildings::NameCustom);
				SAVE_OTHER(Vars::ESP::Buildings::NameColor);
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
				SAVE_VAR(Vars::ESP::Buildings::TeleExitDir);
				SAVE_OTHER(Vars::ESP::Buildings::TeleExitDirColor);
			}

			//World
			{
				SAVE_VAR(Vars::ESP::World::Active);

				SAVE_VAR(Vars::ESP::World::HealthName);
				SAVE_VAR(Vars::ESP::World::HealthLine);
				SAVE_VAR(Vars::ESP::World::HealthBox);

				SAVE_VAR(Vars::ESP::World::AmmoName);
				SAVE_VAR(Vars::ESP::World::AmmoLine);
				SAVE_VAR(Vars::ESP::World::AmmoBox);

				SAVE_VAR(Vars::ESP::World::NPCName);
				SAVE_VAR(Vars::ESP::World::NPCLine);
				SAVE_VAR(Vars::ESP::World::NPCBox);

				SAVE_VAR(Vars::ESP::World::BombName);
				SAVE_VAR(Vars::ESP::World::BombLine);
				SAVE_VAR(Vars::ESP::World::BombBox);

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
				SAVE_VAR(Vars::Chams::Players::Wearables);
				SAVE_VAR(Vars::Chams::Players::Weapons);
				SAVE_VAR(Vars::Chams::Players::FadeoutTeammates);
			}

			//Buildings
			{
				SAVE_VAR(Vars::Chams::Buildings::Active);
				SAVE_VAR(Vars::Chams::Buildings::Material);
				SAVE_VAR(Vars::Chams::Buildings::IgnoreZ);
			}

			//World
			{
				SAVE_VAR(Vars::Chams::World::Active);
			}

			//DME
			{
				SAVE_VAR(Vars::Chams::DME::Active);

				SAVE_VAR(Vars::Chams::DME::HandsGlowOverlay);
				SAVE_VAR(Vars::Chams::DME::WeaponGlowOverlay);
				SAVE_VAR(Vars::Chams::DME::HandsRainbow);
				SAVE_VAR(Vars::Chams::DME::WeaponRainbow);
				SAVE_VAR(Vars::Chams::DME::HandsOverlayRainbow);
				SAVE_VAR(Vars::Chams::DME::WeaponOverlayRainbow);
				SAVE_VAR(Vars::Chams::DME::HandsProxySkin);
				SAVE_VAR(Vars::Chams::DME::WeaponsProxySkin);
				SAVE_VAR(Vars::Chams::DME::HandsGlowAmount);
				SAVE_VAR(Vars::Chams::DME::WeaponGlowAmount);
				SAVE_VAR(Vars::Chams::DME::WeaponOverlayPulse);
				SAVE_VAR(Vars::Chams::DME::HandsOverlayPulse);
			}
		}

		//Glow
		{
			//Main
			{
				SAVE_VAR(Vars::Glow::Main::Active);
				SAVE_VAR(Vars::Glow::Main::Scale);
				SAVE_VAR(Vars::Glow::Main::Type);
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
				SAVE_VAR(Vars::Glow::World::NPCs);
				SAVE_VAR(Vars::Glow::World::Bombs);
				SAVE_VAR(Vars::Glow::World::Projectiles);
				SAVE_VAR(Vars::Glow::World::Alpha);
			}

			//Misc
			{
				SAVE_VAR(Vars::Glow::Misc::MovementSimLine);
				SAVE_VAR(Vars::Glow::Misc::BulletTracers);
				SAVE_VAR(Vars::Glow::Misc::Sightlines);
			}
		}

		//Radar
		{
			//Main
			{
				SAVE_VAR(Vars::Radar::Main::Active);
				SAVE_VAR(Vars::Radar::Main::BackAlpha);
				SAVE_VAR(Vars::Radar::Main::LineAlpha);
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
				SAVE_VAR(Vars::Radar::Players::Height);
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
			SAVE_VAR(Vars::Visuals::CleanScreenshots);
			SAVE_VAR(Vars::Visuals::RemoveMOTD);
			SAVE_VAR(Vars::Visuals::RemoveDisguises);
			SAVE_VAR(Vars::Visuals::DrawOnScreenConditions);
			SAVE_VAR(Vars::Visuals::DrawOnScreenPing);
			SAVE_VAR(Vars::Visuals::RemoveTaunts);
			SAVE_VAR(Vars::Visuals::FieldOfView);
			SAVE_VAR(Vars::Visuals::AimFOVAlpha);
			SAVE_VAR(Vars::Visuals::RemoveScope);
			SAVE_VAR(Vars::Visuals::RemoveRagdolls);
			SAVE_VAR(Vars::Visuals::RemoveScreenEffects);
			SAVE_VAR(Vars::Visuals::RemoveDSP);
			SAVE_VAR(Vars::Visuals::RemoveConvarQueries);
			SAVE_VAR(Vars::Visuals::VisualOverlay);
			SAVE_VAR(Vars::Visuals::RemoveScreenOverlays);
			SAVE_VAR(Vars::Visuals::PreventForcedAngles);
			SAVE_VAR(Vars::Visuals::ScopeLines);
			SAVE_VAR(Vars::Visuals::PickupTimers);
			SAVE_VAR(Vars::Visuals::RemoveZoom);
			SAVE_VAR(Vars::Visuals::RemovePunch);
			SAVE_VAR(Vars::Visuals::CrosshairAimPos);
			SAVE_VAR(Vars::Visuals::ChatInfoText);
			SAVE_VAR(Vars::Visuals::ChatInfoChat);
			SAVE_VAR(Vars::Visuals::ChatInfoGrayScale);
			SAVE_VAR(Vars::Visuals::OutOfFOVArrowsOutline);
			SAVE_VAR(Vars::Visuals::SpectatorList);

			SAVE_VAR(Vars::Visuals::FreecamKey);
			SAVE_VAR(Vars::Visuals::FreecamSpeed);

			SAVE_VAR(Vars::Visuals::CameraMode);
			SAVE_VAR(Vars::Visuals::CameraFOV);

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
			SAVE_VAR(Vars::Visuals::ThirdpersonOffset);
			SAVE_VAR(Vars::Visuals::ThirdpersonDist);
			SAVE_VAR(Vars::Visuals::ThirdpersonRight);
			SAVE_VAR(Vars::Visuals::ThirdpersonUp);
			SAVE_VAR(Vars::Visuals::ThirdpersonOffsetWithArrows);
			SAVE_VAR(Vars::Visuals::ThirdpersonArrowOffsetKey);
			SAVE_VAR(Vars::Visuals::ThirdpersonCrosshair);
			SAVE_VAR(Vars::Visuals::WorldModulation);
			SAVE_VAR(Vars::Visuals::SkyModulation);
			SAVE_VAR(Vars::Visuals::PropWireframe);
			SAVE_VAR(Vars::Visuals::SkyboxChanger);
			SAVE_VAR(Vars::Visuals::BulletTracer);
			SAVE_VAR(Vars::Visuals::BulletTracerRainbow);
			SAVE_VAR(Vars::Visuals::AimbotViewmodel);
			SAVE_VAR(Vars::Visuals::ViewmodelSway);
			SAVE_VAR(Vars::Visuals::MoveSimLine);
			SAVE_VAR(Vars::Visuals::MoveSimSeperators);
			SAVE_VAR(Vars::Visuals::SeperatorLength);
			SAVE_VAR(Vars::Visuals::SeperatorSpacing);
			SAVE_OTHER(Vars::Visuals::VMOffsets);
			SAVE_OTHER(Vars::Visuals::OnScreenConditions);
			SAVE_OTHER(Vars::Visuals::OnScreenPing);
			SAVE_VAR(Vars::Visuals::VMRoll);
			SAVE_VAR(Vars::Visuals::OutOfFOVArrows);
			SAVE_VAR(Vars::Visuals::ArrowLength);
			SAVE_VAR(Vars::Visuals::ArrowAngle);
			SAVE_VAR(Vars::Visuals::MaxDist);
			SAVE_VAR(Vars::Visuals::MinDist);
			SAVE_VAR(Vars::Visuals::FovArrowsDist);
			SAVE_VAR(Vars::Visuals::AimPosSquare);
			SAVE_VAR(Vars::Visuals::Rain);
			SAVE_VAR(Vars::Debug::DebugInfo);
			SAVE_VAR(Vars::Visuals::DoPostProcessing);
			SAVE_VAR(Vars::Visuals::EquipRegionUnlock);

			SAVE_VAR(Vars::Visuals::NoStaticPropFade);
			// Beans I LOVE Beans
			{
				SAVE_VAR(Vars::Visuals::Beans::Active);
				SAVE_VAR(Vars::Visuals::Beans::Rainbow);
				SAVE_OTHER(Vars::Visuals::Beans::BeamColour);
				SAVE_VAR(Vars::Visuals::Beans::UseCustomModel);
				SAVE_STRING(Vars::Visuals::Beans::Model);
				SAVE_VAR(Vars::Visuals::Beans::Life);
				SAVE_VAR(Vars::Visuals::Beans::Width);
				SAVE_VAR(Vars::Visuals::Beans::EndWidth);
				SAVE_VAR(Vars::Visuals::Beans::FadeLength);
				SAVE_VAR(Vars::Visuals::Beans::Amplitude);
				SAVE_VAR(Vars::Visuals::Beans::Brightness);
				SAVE_VAR(Vars::Visuals::Beans::Speed);
				SAVE_VAR(Vars::Visuals::Beans::Flags);
				SAVE_VAR(Vars::Visuals::Beans::Segments);
			}

			SAVE_VAR(Vars::Visuals::NotificationLifetime);
			SAVE_VAR(Vars::Visuals::DamageLoggerText);
			SAVE_VAR(Vars::Visuals::DamageLoggerChat);
			SAVE_VAR(Vars::Visuals::DamageLoggerConsole);
			SAVE_VAR(Vars::Visuals::ParticleTracer);
			SAVE_VAR(Vars::Visuals::VisionModifier);

			{
				SAVE_VAR(Vars::Visuals::RagdollEffects::EnemyOnly);
				SAVE_VAR(Vars::Visuals::RagdollEffects::Burning);
				SAVE_VAR(Vars::Visuals::RagdollEffects::Electrocuted);
				SAVE_VAR(Vars::Visuals::RagdollEffects::BecomeAsh);
				SAVE_VAR(Vars::Visuals::RagdollEffects::Dissolve);
				SAVE_VAR(Vars::Visuals::RagdollEffects::RagdollType);
			}

			{
				SAVE_VAR(Vars::Visuals::Fog::CustomFog);
				SAVE_VAR(Vars::Visuals::Fog::FogDensity);
				SAVE_VAR(Vars::Visuals::Fog::FogDensitySkybox);
				SAVE_VAR(Vars::Visuals::Fog::FogStart);
				SAVE_VAR(Vars::Visuals::Fog::FogStartSkybox);
				SAVE_VAR(Vars::Visuals::Fog::FogEnd);
				SAVE_VAR(Vars::Visuals::Fog::FogEndSkybox);
				SAVE_OTHER(Vars::Visuals::Fog::FogColor);
				SAVE_OTHER(Vars::Visuals::Fog::FogColorSkybox);
			}
		}

		//Misc
		{
			SAVE_VAR(Vars::Misc::FastDeltaStrafe);
			SAVE_VAR(Vars::Misc::AccurateMovement);
			SAVE_VAR(Vars::Misc::DirectionalOnlyOnMove);
			SAVE_VAR(Vars::Misc::AutoJump);
			SAVE_VAR(Vars::Misc::AutoVote);
			SAVE_VAR(Vars::Misc::DuckJump);
			SAVE_VAR(Vars::Misc::TauntSlide);
			SAVE_VAR(Vars::Misc::TauntControl);
			SAVE_VAR(Vars::Misc::TauntSpin);
			SAVE_VAR(Vars::Misc::TauntSpinKey);
			SAVE_VAR(Vars::Misc::TauntSpinSpeed);
			SAVE_VAR(Vars::Misc::TauntFollowsCamera);
			SAVE_VAR(Vars::Misc::BypassPure);
			SAVE_VAR(Vars::Misc::NoisemakerSpam);
			SAVE_VAR(Vars::Misc::DisableInterpolation);
			SAVE_VAR(Vars::Misc::MedalFlip);
			SAVE_VAR(Vars::Misc::AutoRocketJump);
			SAVE_VAR(Vars::Misc::NonLethalRocketJump);
			SAVE_VAR(Vars::Misc::AutoScoutJump);
			SAVE_VAR(Vars::Misc::ChatSpam);
			SAVE_VAR(Vars::Misc::NoPush);
			SAVE_VAR(Vars::Misc::AutoStrafe);
			SAVE_VAR(Vars::Misc::Directional);
			SAVE_VAR(Vars::Misc::EdgeJump);
			SAVE_VAR(Vars::Misc::EdgeJumpKey);
			SAVE_VAR(Vars::Misc::StoreStatistics);
			SAVE_VAR(Vars::Misc::AntiAFK);
			SAVE_VAR(Vars::Misc::CheatsBypass);
			SAVE_VAR(Vars::Misc::RageRetry);
			SAVE_VAR(Vars::Misc::RageRetryHealth);
			SAVE_VAR(Vars::Misc::MVMRes);
			SAVE_VAR(Vars::Misc::VotingOptions);
			SAVE_VAR(Vars::Misc::PingReducer);
			SAVE_VAR(Vars::Misc::PingTarget);
			SAVE_VAR(Vars::Misc::ExtendFreeze);
			SAVE_VAR(Vars::Misc::ViewmodelFlip);
			SAVE_VAR(Vars::Misc::AutoJoin);
			SAVE_VAR(Vars::Misc::KillstreakWeapon);
			SAVE_VAR(Vars::Misc::PartyNetworking);
			SAVE_VAR(Vars::Misc::PartyMarker);
			SAVE_VAR(Vars::Misc::PartyESP);
			SAVE_VAR(Vars::Misc::CrouchSpeed);
			SAVE_VAR(Vars::Misc::FastAccel);
			SAVE_VAR(Vars::Misc::FakeAccelAngle);
			SAVE_VAR(Vars::Misc::SoundBlock);
			SAVE_VAR(Vars::Misc::ChatFlags);
			SAVE_VAR(Vars::Misc::MedievalChat);
			SAVE_VAR(Vars::Misc::AutoAcceptItemDrops);
			SAVE_VAR(Vars::Misc::RegionChanger);
			SAVE_VAR(Vars::Misc::RegionsAllowed);
			SAVE_VAR(Vars::Misc::AutoCasualQueue);
			SAVE_VAR(Vars::Misc::AntiVAC);
			SAVE_VAR(Vars::Misc::InstantAccept);
			SAVE_VAR(Vars::Misc::RunescapeChat);
			SAVE_VAR(Vars::Misc::AntiAutobal);
			SAVE_VAR(Vars::Misc::AntiViewmodelFlip);
			SAVE_VAR(Vars::Visuals::ParticlesIgnoreZ);

			// Followbot
			{
				SAVE_VAR(Vars::Misc::Followbot::Enabled);
				SAVE_VAR(Vars::Misc::Followbot::Distance);
			}

			// Cheater Detection
			{
				SAVE_VAR(Vars::Misc::CheaterDetection::Enabled);
				SAVE_VAR(Vars::Misc::CheaterDetection::Methods);
				SAVE_VAR(Vars::Misc::CheaterDetection::Protections);
				SAVE_VAR(Vars::Misc::CheaterDetection::SuspicionGate);
				SAVE_VAR(Vars::Misc::CheaterDetection::PacketManipGate);
				SAVE_VAR(Vars::Misc::CheaterDetection::BHopMaxDelay);
				SAVE_VAR(Vars::Misc::CheaterDetection::BHopDetectionsRequired);
				SAVE_VAR(Vars::Misc::CheaterDetection::ScoreMultiplier);
				SAVE_VAR(Vars::Misc::CheaterDetection::MinimumFlickDistance);
				SAVE_VAR(Vars::Misc::CheaterDetection::MaximumNoise);
				SAVE_VAR(Vars::Misc::CheaterDetection::MinimumAimbotFoV);
				SAVE_VAR(Vars::Misc::CheaterDetection::MaxScaledAimbotFoV);
			}

			// CL_Move
			{
				SAVE_VAR(Vars::Misc::CL_Move::Enabled); //Enabled
				SAVE_VAR(Vars::Misc::CL_Move::Doubletap); // { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::SafeTick);
				SAVE_VAR(Vars::Misc::CL_Move::SafeTickAirOverride);
				SAVE_VAR(Vars::Misc::CL_Move::PassiveRecharge);
				SAVE_VAR(Vars::Misc::CL_Move::WaitForDT); // { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::NotInAir); // { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::StopMovement);
				SAVE_VAR(Vars::Misc::CL_Move::DoubletapKey); // { true, L"Doubletap" };
				SAVE_VAR(Vars::Misc::CL_Move::TeleportKey); // { 0x46, L"Teleport Key" }; //F
				SAVE_VAR(Vars::Misc::CL_Move::TeleportMode);
				SAVE_VAR(Vars::Misc::CL_Move::TeleportFactor);
				SAVE_VAR(Vars::Misc::CL_Move::RechargeKey); // { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::RetainFakelag);
				SAVE_VAR(Vars::Misc::CL_Move::RechargeWhileDead);
				SAVE_VAR(Vars::Misc::CL_Move::AutoRecharge);
				SAVE_VAR(Vars::Misc::CL_Move::AntiWarp);
				SAVE_VAR(Vars::Misc::CL_Move::DTBarStyle);
				SAVE_OTHER(Vars::Misc::CL_Move::DTIndicator);
				SAVE_VAR(Vars::Misc::CL_Move::DTMode);
				SAVE_VAR(Vars::Misc::CL_Move::Fakelag); // { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::FakelagMin);
				SAVE_VAR(Vars::Misc::CL_Move::FakelagMax);
				SAVE_VAR(Vars::Misc::CL_Move::FakelagMode);
				SAVE_VAR(Vars::Misc::CL_Move::WhileMoving);
				SAVE_VAR(Vars::Misc::CL_Move::WhileInAir);
				SAVE_VAR(Vars::Misc::CL_Move::WhileUnducking);
				SAVE_VAR(Vars::Misc::CL_Move::WhileVisible);
				SAVE_VAR(Vars::Misc::CL_Move::PredictVisibility);
				SAVE_VAR(Vars::Misc::CL_Move::FakelagOnKey); // { 0x52, L"Recharge Key" }; //
				SAVE_VAR(Vars::Misc::CL_Move::FakelagKey); // { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::FakelagValue); // { 0x52, L"Recharge Key" }; //R
				SAVE_VAR(Vars::Misc::CL_Move::DTTicks);
				SAVE_VAR(Vars::Misc::CL_Move::AutoPeekKey);
				SAVE_VAR(Vars::Misc::CL_Move::AutoPeekDistance);
				SAVE_VAR(Vars::Misc::CL_Move::AutoPeekFree);
				{
					SAVE_VAR(Vars::Misc::CL_Move::FLGChams::Material);
					SAVE_OTHER(Vars::Misc::CL_Move::FLGChams::FakelagColor);
				}
			}
			//Discord
			{
				SAVE_VAR(Vars::Misc::Discord::EnableRPC);
				SAVE_VAR(Vars::Misc::Discord::IncludeMap);
				SAVE_VAR(Vars::Misc::Discord::IncludeClass);
				SAVE_VAR(Vars::Misc::Discord::IncludeTimestamp);
				SAVE_VAR(Vars::Misc::Discord::WhatImagesShouldBeUsed);
			}
			//Steam
			{
				SAVE_VAR(Vars::Misc::Steam::EnableRPC);
				SAVE_VAR(Vars::Misc::Steam::MatchGroup);
				SAVE_VAR(Vars::Misc::Steam::OverrideMenu);
				SAVE_VAR(Vars::Misc::Steam::MapText);
				SAVE_VAR(Vars::Misc::Steam::GroupSize);
				SAVE_VAR(Vars::Misc::Steam::CustomText);
			}
		}

		//AntiHack
		{
			//AntiAim
			{
				SAVE_VAR(Vars::AntiHack::AntiAim::Active);
				SAVE_VAR(Vars::AntiHack::AntiAim::ToggleKey);
				SAVE_VAR(Vars::AntiHack::AntiAim::InvertKey);
				SAVE_VAR(Vars::AntiHack::AntiAim::ManualKey);
				SAVE_VAR(Vars::AntiHack::AntiAim::Pitch);
				SAVE_VAR(Vars::AntiHack::AntiAim::BaseYawMode);
				SAVE_VAR(Vars::AntiHack::AntiAim::YawReal);
				SAVE_VAR(Vars::AntiHack::AntiAim::YawFake);
				SAVE_VAR(Vars::AntiHack::AntiAim::SpinSpeed);
				SAVE_VAR(Vars::AntiHack::AntiAim::CustomRealPitch);
				SAVE_VAR(Vars::AntiHack::AntiAim::BaseYawOffset);
				SAVE_VAR(Vars::AntiHack::AntiAim::CustomRealYaw);
				SAVE_VAR(Vars::AntiHack::AntiAim::CustomFakeYaw);
				SAVE_VAR(Vars::AntiHack::AntiAim::RealJitter);
				SAVE_VAR(Vars::AntiHack::AntiAim::FakeJitter);
				SAVE_VAR(Vars::AntiHack::AntiAim::RandInterval);
				SAVE_VAR(Vars::AntiHack::AntiAim::AntiOverlap);
				SAVE_VAR(Vars::AntiHack::AntiAim::AntiBackstab);
				SAVE_VAR(Vars::AntiHack::AntiAim::LegJitter);
				SAVE_VAR(Vars::AntiHack::AntiAim::InvalidShootPitch);
			}
			//Resolver
			{
				SAVE_VAR(Vars::AntiHack::Resolver::Resolver);
				SAVE_VAR(Vars::AntiHack::Resolver::IgnoreAirborne);
			}
		}

		//Others
		{
			SAVE_OTHER(Vars::Menu::Colors::MenuAccent);

			SAVE_OTHER(Colors::OutlineESP);
			SAVE_OTHER(Colors::DTBarIndicatorsCharged);
			SAVE_OTHER(Colors::DTBarIndicatorsCharging);
			SAVE_OTHER(Colors::ChokedBar);
			SAVE_OTHER(Colors::GradientHealthBar);
			SAVE_OTHER(Colors::GradientOverhealBar);
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
			SAVE_OTHER(Colors::Enemy);
			SAVE_OTHER(Colors::rTeam);
			SAVE_OTHER(Colors::TeamRed);
			SAVE_OTHER(Colors::TeamBlu);
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
			SAVE_OTHER(Colors::FresnelBaseHands);
			SAVE_OTHER(Colors::FresnelBaseWeps);
			SAVE_OTHER(Colors::FresnelTop);
			SAVE_OTHER(Colors::AimSquareCol);
			SAVE_OTHER(Colors::DtOutline);
			SAVE_OTHER(Colors::NotifBG);
			SAVE_OTHER(Colors::NotifOutline);
			SAVE_OTHER(Colors::NotifText);
			SAVE_OTHER(Colors::WeaponIcon);
			SAVE_OTHER(Colors::NoscopeLines1);
			SAVE_OTHER(Colors::NoscopeLines2);
			SAVE_OTHER(Colors::bonecolor);
			SAVE_OTHER(Colors::HitboxFace);
			SAVE_OTHER(Colors::HitboxEdge);

			SAVE_OTHER(Vars::Skybox::SkyboxNum);
			SAVE_STRING(Vars::Skybox::SkyboxName);

			SAVE_OTHER(Vars::Chams::Players::Local);
			SAVE_OTHER(Vars::Chams::Players::FakeAng);
			SAVE_OTHER(Vars::Chams::Players::Enemy);
			SAVE_OTHER(Vars::Chams::Players::Team);
			SAVE_OTHER(Vars::Chams::Players::Friend);
			SAVE_OTHER(Vars::Chams::Players::Target);
			SAVE_OTHER(Vars::Chams::Players::Ragdoll);
			SAVE_OTHER(Vars::Chams::DME::Hands);
			SAVE_OTHER(Vars::Chams::DME::Weapon);

			SAVE_OTHER(Vars::Chams::Buildings::Local);
			SAVE_OTHER(Vars::Chams::Buildings::Enemy);
			SAVE_OTHER(Vars::Chams::Buildings::Team);
			SAVE_OTHER(Vars::Chams::Buildings::Friend);
			SAVE_OTHER(Vars::Chams::Buildings::Target);

			SAVE_OTHER(Vars::Chams::World::Health);
			SAVE_OTHER(Vars::Chams::World::Ammo);
			SAVE_OTHER(Vars::Chams::World::Projectiles);

			SAVE_OTHER(Vars::Menu::ModernDesign);
			SAVE_OTHER(Vars::Menu::ShowPlayerlist);
			SAVE_OTHER(Vars::Menu::ShowKeybinds);
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

		write_json(ConfigPath + "\\" + configName + ConfigExtension, WriteTree);
		F::Notifications.Add("Config " + configName + " saved");
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CConfigManager::LoadConfig(const std::string& configName)
{
	// Read ptree from json
	try
	{
		ReadTree.clear();
		read_json(ConfigPath + "\\" + configName + ConfigExtension, ReadTree);


		// Menu
		{
			LOAD_OTHER(Vars::Menu::CheatName);
			LOAD_OTHER(Vars::Menu::CheatPrefix);
			LOAD_VAR(Vars::Menu::Vignette);
			LOAD_VAR(Vars::Menu::ShowDVD);
			LOAD_VAR(Vars::Menu::CloseOnUnfocus);
			LOAD_VAR(Vars::Menu::MenuKey);
		}

		//Aimbot
		{
			//Global
			{
				LOAD_VAR(Vars::Aimbot::Global::Active);
				LOAD_VAR(Vars::Aimbot::Global::AimKey);
				LOAD_VAR(Vars::Aimbot::Global::AimFOV);
				LOAD_VAR(Vars::Aimbot::Global::AutoShoot);
				LOAD_VAR(Vars::Aimbot::Global::DontWaitForShot);
				LOAD_VAR(Vars::Aimbot::Global::FlickatEnemies);
				LOAD_VAR(Vars::Aimbot::Global::AimPlayers);
				LOAD_VAR(Vars::Aimbot::Global::AimBuildings);
				LOAD_VAR(Vars::Aimbot::Global::AimStickies);
				LOAD_VAR(Vars::Aimbot::Global::AimNPC);
				LOAD_VAR(Vars::Aimbot::Global::AimBombs);
				LOAD_VAR(Vars::Aimbot::Global::IgnoreOptions);
				LOAD_VAR(Vars::Aimbot::Global::BAimLethal);
				LOAD_VAR(Vars::Aimbot::Global::showHitboxes);
				LOAD_VAR(Vars::Aimbot::Global::ClearPreviousHitbox);
				LOAD_VAR(Vars::Aimbot::Global::HitboxLifetime);
			}


			// Backtrack
			{
				LOAD_VAR(Vars::Backtrack::Enabled);
				LOAD_VAR(Vars::Backtrack::Latency);
				//Bt Chams
				{
					LOAD_VAR(Vars::Backtrack::BtChams::Enabled);
					LOAD_VAR(Vars::Backtrack::BtChams::LastOnly);
					LOAD_VAR(Vars::Backtrack::BtChams::EnemyOnly);
					LOAD_VAR(Vars::Backtrack::BtChams::IgnoreZ);
					LOAD_VAR(Vars::Backtrack::BtChams::Material);
					LOAD_OTHER(Vars::Backtrack::BtChams::BacktrackColor);
				}
			}

			{
				LOAD_VAR(Vars::CritHack::Active);
				LOAD_VAR(Vars::CritHack::Indicators);
				LOAD_OTHER(Vars::CritHack::IndicatorPos);
				LOAD_VAR(Vars::CritHack::AvoidRandom);
				LOAD_VAR(Vars::CritHack::AlwaysMelee);
				LOAD_VAR(Vars::CritHack::CritKey);
				LOAD_VAR(Vars::CritHack::AutoMeleeCrit);
			}

			//Hitscan
			{
				//LOAD_VAR(Vars::Aimbot::Hitscan::Active);
				LOAD_VAR(Vars::Aimbot::Hitscan::SortMethod);
				LOAD_VAR(Vars::Aimbot::Hitscan::BackTrackMethod);
				LOAD_VAR(Vars::Aimbot::Hitscan::AimMethod);
				LOAD_VAR(Vars::Aimbot::Hitscan::AimHitbox);
				//LOAD_VAR(Vars::Aimbot::Hitscan::AimFOV);
				LOAD_VAR(Vars::Aimbot::Hitscan::RespectFOV);
				LOAD_VAR(Vars::Aimbot::Hitscan::SmoothingAmount);
				LOAD_VAR(Vars::Aimbot::Hitscan::TapFire);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScanHitboxes);
				LOAD_VAR(Vars::Aimbot::Hitscan::MultiHitboxes);
				LOAD_VAR(Vars::Aimbot::Hitscan::StaticHitboxes);
				LOAD_VAR(Vars::Aimbot::Hitscan::PointScale);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScanBuildings);
				LOAD_VAR(Vars::Aimbot::Hitscan::WaitForHeadshot);
				LOAD_VAR(Vars::Aimbot::Hitscan::WaitForCharge);
				LOAD_VAR(Vars::Aimbot::Hitscan::SpectatedSmooth);
				LOAD_VAR(Vars::Aimbot::Hitscan::ScopedOnly);
				LOAD_VAR(Vars::Aimbot::Hitscan::AutoScope);
				LOAD_VAR(Vars::Aimbot::Hitscan::AutoRev);
				LOAD_VAR(Vars::Aimbot::Hitscan::ExtinguishTeam);
			}

			//Projectile
			{
				//LOAD_VAR(Vars::Aimbot::Projectile::Active);
				LOAD_VAR(Vars::Aimbot::Projectile::SortMethod);
				LOAD_VAR(Vars::Aimbot::Projectile::AimMethod);
				LOAD_VAR(Vars::Aimbot::Projectile::AimPosition);
				LOAD_VAR(Vars::Aimbot::Projectile::RespectFOV);
				LOAD_VAR(Vars::Aimbot::Projectile::VisTestPoints);
				LOAD_VAR(Vars::Aimbot::Projectile::AllowedHitboxes);
				LOAD_VAR(Vars::Aimbot::Projectile::FeetAimIfOnGround);
				LOAD_VAR(Vars::Aimbot::Projectile::BounceKey);
				LOAD_VAR(Vars::Aimbot::Projectile::SplashPrediction);
				LOAD_OTHER(Vars::Aimbot::Projectile::PredictionColor);
				LOAD_VAR(Vars::Aimbot::Projectile::PredictionTime);
				LOAD_VAR(Vars::Aimbot::Projectile::ScanPoints);
				LOAD_VAR(Vars::Aimbot::Projectile::ScanScale);
				LOAD_VAR(Vars::Aimbot::Projectile::PredictObscured);
				LOAD_VAR(Vars::Aimbot::Projectile::NoSpread);
				LOAD_VAR(Vars::Aimbot::Projectile::ChargeLooseCannon);
				LOAD_VAR(Vars::Aimbot::Projectile::StrafePredictionAir);
				LOAD_VAR(Vars::Aimbot::Projectile::StrafePredictionGround);
				LOAD_VAR(Vars::Aimbot::Projectile::StrafePredictionSamples);
				LOAD_VAR(Vars::Aimbot::Projectile::StrafePredictionMinDifference);
				LOAD_VAR(Vars::Aimbot::Projectile::StrafePredictionMaxDistance);
				LOAD_VAR(Vars::Aimbot::Projectile::MinSplashPredictionDistance);
				LOAD_VAR(Vars::Aimbot::Projectile::MaxSplashPredictionDistance);
				LOAD_VAR(Vars::Aimbot::Projectile::WaitForHit);
				//LOAD_VAR(Vars::Aimbot::Projectile::AimFOV);
			}

			//Melee
			{
				//LOAD_VAR(Vars::Aimbot::Melee::Active);
				LOAD_VAR(Vars::Aimbot::Melee::SortMethod);
				LOAD_VAR(Vars::Aimbot::Melee::AimMethod);
				//LOAD_VAR(Vars::Aimbot::Melee::AimFOV);
				LOAD_VAR(Vars::Aimbot::Melee::RespectFOV);
				LOAD_VAR(Vars::Aimbot::Melee::SmoothingAmount);
				LOAD_VAR(Vars::Aimbot::Melee::RangeCheck);
				LOAD_VAR(Vars::Aimbot::Melee::PredictSwing);
				LOAD_VAR(Vars::Aimbot::Melee::WhipTeam);
			}
		}

		//Triggerbot
		{
			//Global
			{
				LOAD_VAR(Vars::Triggerbot::Global::Active);
				LOAD_VAR(Vars::Triggerbot::Global::TriggerKey);
				LOAD_VAR(Vars::Triggerbot::Global::IgnoreOptions);
			}

			//Shoot
			{
				LOAD_VAR(Vars::Triggerbot::Shoot::Active);
				LOAD_VAR(Vars::Triggerbot::Shoot::TriggerPlayers);
				LOAD_VAR(Vars::Triggerbot::Shoot::TriggerBuildings);
				LOAD_VAR(Vars::Triggerbot::Shoot::HeadOnly);
				LOAD_VAR(Vars::Triggerbot::Shoot::WaitForHeadshot);
				LOAD_VAR(Vars::Triggerbot::Shoot::ScopeOnly);
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
				LOAD_VAR(Vars::Triggerbot::Detonate::DetonateTargets);
				LOAD_VAR(Vars::Triggerbot::Detonate::Stickies);
				LOAD_VAR(Vars::Triggerbot::Detonate::Flares);
				LOAD_VAR(Vars::Triggerbot::Detonate::RadiusScale);
			}

			//Blast
			{
				LOAD_VAR(Vars::Triggerbot::Blast::Active);
				LOAD_VAR(Vars::Triggerbot::Blast::Rage);
				LOAD_VAR(Vars::Triggerbot::Blast::Silent);
				LOAD_VAR(Vars::Triggerbot::Blast::ExtinguishPlayers);
				LOAD_VAR(Vars::Triggerbot::Blast::DisableOnAttack);
				LOAD_VAR(Vars::Triggerbot::Blast::Fov);
			}

			//Uber
			{
				LOAD_VAR(Vars::Triggerbot::Uber::Active);
				LOAD_VAR(Vars::Triggerbot::Uber::OnlyFriends);
				LOAD_VAR(Vars::Triggerbot::Uber::PopLocal);
				LOAD_VAR(Vars::Triggerbot::Uber::AutoVacc);
				LOAD_VAR(Vars::Triggerbot::Uber::HealthLeft);
				LOAD_VAR(Vars::Triggerbot::Uber::VoiceCommand);
				LOAD_VAR(Vars::Triggerbot::Uber::ReactFoV);
				LOAD_VAR(Vars::Triggerbot::Uber::ReactClasses);
				LOAD_VAR(Vars::Triggerbot::Uber::BlastRes);
				LOAD_VAR(Vars::Triggerbot::Uber::BulletRes);
				LOAD_VAR(Vars::Triggerbot::Uber::FireRes);
			}
		}

		//ESP
		{
			//Main
			{
				LOAD_VAR(Vars::ESP::Main::Active);
				LOAD_VAR(Vars::ESP::Main::Outlinedbar);
				LOAD_VAR(Vars::ESP::Main::EnableTeamEnemyColors);
				LOAD_VAR(Vars::ESP::Main::DistanceToAlpha);
				LOAD_VAR(Vars::ESP::Main::DormantSoundESP);
				LOAD_VAR(Vars::ESP::Main::DormantTime);
				LOAD_VAR(Vars::ESP::Main::DormantDist);
				LOAD_VAR(Vars::ESP::Main::NetworkedDist);
			}

			//Players
			{
				LOAD_VAR(Vars::ESP::Players::Active);
				LOAD_VAR(Vars::ESP::Players::ShowLocal);
				LOAD_VAR(Vars::ESP::Players::IgnoreTeammates);
				LOAD_VAR(Vars::ESP::Players::IgnoreCloaked);
				LOAD_VAR(Vars::ESP::Players::Name);
				LOAD_VAR(Vars::ESP::Players::NameCustom);
				LOAD_OTHER(Vars::ESP::Players::NameColor);
				LOAD_VAR(Vars::ESP::Players::NameBox);
				LOAD_VAR(Vars::ESP::Players::Uber);
				LOAD_VAR(Vars::ESP::Players::Class);
				LOAD_VAR(Vars::ESP::Players::HealthText);
				LOAD_VAR(Vars::ESP::Players::Cond);
				LOAD_VAR(Vars::ESP::Players::HealthBar);
				LOAD_VAR(Vars::ESP::Players::HealthBarStyle);
				LOAD_VAR(Vars::ESP::Players::Box);
				LOAD_VAR(Vars::ESP::Players::Choked);
				LOAD_VAR(Vars::ESP::Players::GUID);
				LOAD_VAR(Vars::ESP::Players::Alpha);
				LOAD_VAR(Vars::ESP::Players::Lines);
				LOAD_VAR(Vars::ESP::Players::Bones);
				LOAD_VAR(Vars::ESP::Players::Dlights);
				LOAD_VAR(Vars::ESP::Players::DlightRadius);
				LOAD_VAR(Vars::ESP::Players::CheaterDetection);
				LOAD_VAR(Vars::ESP::Players::WeaponIcon);
				LOAD_VAR(Vars::ESP::Players::WeaponText);
				LOAD_VAR(Vars::ESP::Players::SniperSightlines);
			}

			//Buildings
			{
				LOAD_VAR(Vars::ESP::Buildings::Active);
				LOAD_VAR(Vars::ESP::Buildings::IgnoreTeammates);
				LOAD_VAR(Vars::ESP::Buildings::Name);
				LOAD_VAR(Vars::ESP::Buildings::NameCustom);
				LOAD_OTHER(Vars::ESP::Buildings::NameColor);
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
				LOAD_VAR(Vars::ESP::Buildings::TeleExitDir);
				LOAD_OTHER(Vars::ESP::Buildings::TeleExitDirColor);
			}

			//World
			{
				LOAD_VAR(Vars::ESP::World::Active);

				LOAD_VAR(Vars::ESP::World::HealthName);
				LOAD_VAR(Vars::ESP::World::HealthLine);
				LOAD_VAR(Vars::ESP::World::HealthBox);

				LOAD_VAR(Vars::ESP::World::AmmoName);
				LOAD_VAR(Vars::ESP::World::AmmoLine);
				LOAD_VAR(Vars::ESP::World::AmmoBox);

				LOAD_VAR(Vars::ESP::World::NPCName);
				LOAD_VAR(Vars::ESP::World::NPCLine);
				LOAD_VAR(Vars::ESP::World::NPCBox);

				LOAD_VAR(Vars::ESP::World::BombName);
				LOAD_VAR(Vars::ESP::World::BombLine);
				LOAD_VAR(Vars::ESP::World::BombBox);

				LOAD_VAR(Vars::ESP::World::Alpha);
			}

			//Misc
			{
				LOAD_VAR(Vars::Glow::Misc::MovementSimLine);
				LOAD_VAR(Vars::Glow::Misc::BulletTracers);
				LOAD_VAR(Vars::Glow::Misc::Sightlines);
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
				LOAD_VAR(Vars::Chams::Players::Wearables);
				LOAD_VAR(Vars::Chams::Players::Weapons);
				LOAD_VAR(Vars::Chams::Players::FadeoutTeammates);
			}

			//Buildings
			{
				LOAD_VAR(Vars::Chams::Buildings::Active);
				LOAD_VAR(Vars::Chams::Buildings::Material);
				LOAD_VAR(Vars::Chams::Buildings::IgnoreZ);
			}

			//World
			{
				LOAD_VAR(Vars::Chams::World::Active);
			}

			//DME
			{
				LOAD_VAR(Vars::Chams::DME::Active);
				LOAD_VAR(Vars::Chams::DME::HandsGlowOverlay);
				LOAD_VAR(Vars::Chams::DME::WeaponGlowOverlay);
				LOAD_VAR(Vars::Chams::DME::HandsRainbow);
				LOAD_VAR(Vars::Chams::DME::WeaponRainbow);
				LOAD_VAR(Vars::Chams::DME::HandsOverlayRainbow);
				LOAD_VAR(Vars::Chams::DME::WeaponOverlayRainbow);
				LOAD_VAR(Vars::Chams::DME::HandsProxySkin);
				LOAD_VAR(Vars::Chams::DME::WeaponsProxySkin);
				LOAD_VAR(Vars::Chams::DME::HandsGlowAmount);
				LOAD_VAR(Vars::Chams::DME::WeaponGlowAmount);
				LOAD_VAR(Vars::Chams::DME::WeaponOverlayPulse);
				LOAD_VAR(Vars::Chams::DME::HandsOverlayPulse);
			}
		}

		//Glow
		{
			//Main
			{
				LOAD_VAR(Vars::Glow::Main::Active);
				LOAD_VAR(Vars::Glow::Main::Scale);
				LOAD_VAR(Vars::Glow::Main::Type);
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
				LOAD_VAR(Vars::Glow::World::NPCs);
				LOAD_VAR(Vars::Glow::World::Bombs);
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
				LOAD_VAR(Vars::Radar::Players::Height);
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
			LOAD_VAR(Vars::Visuals::CleanScreenshots);
			LOAD_VAR(Vars::Visuals::RemoveMOTD);
			LOAD_VAR(Vars::Visuals::RemoveDisguises);
			LOAD_VAR(Vars::Visuals::DrawOnScreenConditions);
			LOAD_VAR(Vars::Visuals::DrawOnScreenPing);
			LOAD_VAR(Vars::Visuals::RemoveTaunts);
			LOAD_VAR(Vars::Visuals::RemoveScreenEffects);
			LOAD_VAR(Vars::Visuals::RemoveScreenOverlays);
			LOAD_VAR(Vars::Visuals::RemoveDSP);
			LOAD_VAR(Vars::Visuals::RemoveConvarQueries);
			LOAD_VAR(Vars::Visuals::VisualOverlay);
			LOAD_VAR(Vars::Visuals::PreventForcedAngles);
			LOAD_VAR(Vars::Visuals::FieldOfView);
			LOAD_VAR(Vars::Visuals::AimFOVAlpha);
			LOAD_VAR(Vars::Visuals::RemoveScope);
			LOAD_VAR(Vars::Visuals::RemoveRagdolls);
			LOAD_VAR(Vars::Visuals::PickupTimers);
			LOAD_VAR(Vars::Visuals::ScopeLines);
			LOAD_VAR(Vars::Visuals::RemoveZoom);
			LOAD_VAR(Vars::Visuals::RemovePunch);
			LOAD_VAR(Vars::Visuals::CrosshairAimPos);
			LOAD_VAR(Vars::Visuals::ChatInfoText);
			LOAD_VAR(Vars::Visuals::ChatInfoChat);
			LOAD_VAR(Vars::Visuals::ChatInfoGrayScale);
			LOAD_VAR(Vars::Visuals::OutOfFOVArrowsOutline);
			LOAD_VAR(Vars::Visuals::SpectatorList);

			LOAD_VAR(Vars::Visuals::FreecamKey);
			LOAD_VAR(Vars::Visuals::FreecamSpeed);

			LOAD_VAR(Vars::Visuals::CameraMode);
			LOAD_VAR(Vars::Visuals::CameraFOV);

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
			LOAD_VAR(Vars::Visuals::ThirdpersonOffset);
			LOAD_VAR(Vars::Visuals::ThirdpersonDist);
			LOAD_VAR(Vars::Visuals::ThirdpersonRight);
			LOAD_VAR(Vars::Visuals::ThirdpersonUp);
			LOAD_VAR(Vars::Visuals::ThirdpersonOffsetWithArrows);
			LOAD_VAR(Vars::Visuals::ThirdpersonArrowOffsetKey);
			LOAD_VAR(Vars::Visuals::ThirdpersonCrosshair);

			LOAD_VAR(Vars::Visuals::WorldModulation);
			LOAD_VAR(Vars::Visuals::SkyModulation);
			LOAD_VAR(Vars::Visuals::PropWireframe);
			LOAD_VAR(Vars::Visuals::SkyboxChanger);
			LOAD_VAR(Vars::Visuals::ParticleTracer);
			LOAD_VAR(Vars::Visuals::BulletTracer);
			LOAD_VAR(Vars::Visuals::BulletTracerRainbow);
			LOAD_VAR(Vars::Visuals::AimbotViewmodel);
			LOAD_VAR(Vars::Visuals::ViewmodelSway);
			LOAD_VAR(Vars::Visuals::MoveSimLine);
			LOAD_VAR(Vars::Visuals::MoveSimSeperators);
			LOAD_VAR(Vars::Visuals::SeperatorLength);
			LOAD_VAR(Vars::Visuals::SeperatorSpacing);
			LOAD_OTHER(Vars::Visuals::VMOffsets);
			LOAD_OTHER(Vars::Visuals::OnScreenConditions);
			LOAD_OTHER(Vars::Visuals::OnScreenPing);
			LOAD_VAR(Vars::Visuals::VMRoll);
			LOAD_VAR(Vars::Visuals::OutOfFOVArrows);
			LOAD_VAR(Vars::Visuals::ArrowLength);
			LOAD_VAR(Vars::Visuals::ArrowAngle);
			LOAD_VAR(Vars::Visuals::MaxDist);
			LOAD_VAR(Vars::Visuals::MinDist);
			LOAD_VAR(Vars::Visuals::FovArrowsDist);
			LOAD_VAR(Vars::Visuals::AimPosSquare);
			LOAD_VAR(Vars::Visuals::NotificationLifetime);
			LOAD_VAR(Vars::Visuals::DamageLoggerText);
			LOAD_VAR(Vars::Visuals::DamageLoggerChat);
			LOAD_VAR(Vars::Visuals::DamageLoggerConsole);
			LOAD_VAR(Vars::Visuals::VisionModifier);
			LOAD_VAR(Vars::Visuals::Rain);
			LOAD_VAR(Vars::Debug::DebugInfo);
			LOAD_VAR(Vars::Visuals::DoPostProcessing);
			LOAD_VAR(Vars::Visuals::EquipRegionUnlock);

			LOAD_VAR(Vars::Visuals::NoStaticPropFade);
			LOAD_VAR(Vars::Misc::AntiViewmodelFlip);
			LOAD_VAR(Vars::Visuals::ParticlesIgnoreZ);

			// Beans I LOVE Beans
			{
				LOAD_VAR(Vars::Visuals::Beans::Active);
				LOAD_VAR(Vars::Visuals::Beans::Rainbow);
				LOAD_OTHER(Vars::Visuals::Beans::BeamColour);
				LOAD_VAR(Vars::Visuals::Beans::UseCustomModel);
				LOAD_STRING(Vars::Visuals::Beans::Model);
				LOAD_VAR(Vars::Visuals::Beans::Life);
				LOAD_VAR(Vars::Visuals::Beans::Width);
				LOAD_VAR(Vars::Visuals::Beans::EndWidth);
				LOAD_VAR(Vars::Visuals::Beans::FadeLength);
				LOAD_VAR(Vars::Visuals::Beans::Amplitude);
				LOAD_VAR(Vars::Visuals::Beans::Brightness);
				LOAD_VAR(Vars::Visuals::Beans::Speed);
				LOAD_VAR(Vars::Visuals::Beans::Flags);
				LOAD_VAR(Vars::Visuals::Beans::Segments);
			}

			{
				LOAD_VAR(Vars::Visuals::RagdollEffects::EnemyOnly);
				LOAD_VAR(Vars::Visuals::RagdollEffects::Burning);
				LOAD_VAR(Vars::Visuals::RagdollEffects::Electrocuted);
				LOAD_VAR(Vars::Visuals::RagdollEffects::BecomeAsh);
				LOAD_VAR(Vars::Visuals::RagdollEffects::Dissolve);
				LOAD_VAR(Vars::Visuals::RagdollEffects::RagdollType);
			}

			{
				LOAD_VAR(Vars::Visuals::Fog::CustomFog);
				LOAD_VAR(Vars::Visuals::Fog::FogDensity);
				LOAD_VAR(Vars::Visuals::Fog::FogDensitySkybox);
				LOAD_VAR(Vars::Visuals::Fog::FogStart);
				LOAD_VAR(Vars::Visuals::Fog::FogStartSkybox);
				LOAD_VAR(Vars::Visuals::Fog::FogEnd);
				LOAD_VAR(Vars::Visuals::Fog::FogEndSkybox);
				LOAD_OTHER(Vars::Visuals::Fog::FogColor);
				LOAD_OTHER(Vars::Visuals::Fog::FogColorSkybox);
			}
		}

		//Misc
		{
			LOAD_VAR(Vars::Misc::FastDeltaStrafe);
			LOAD_VAR(Vars::Misc::AccurateMovement);
			LOAD_VAR(Vars::Misc::DirectionalOnlyOnMove);
			LOAD_VAR(Vars::Misc::AutoJump);
			LOAD_VAR(Vars::Misc::AutoVote);
			LOAD_VAR(Vars::Misc::DuckJump);
			LOAD_VAR(Vars::Misc::TauntSlide);
			LOAD_VAR(Vars::Misc::TauntControl);
			LOAD_VAR(Vars::Misc::TauntSpin);
			LOAD_VAR(Vars::Misc::TauntSpinKey);
			LOAD_VAR(Vars::Misc::TauntSpinSpeed);
			LOAD_VAR(Vars::Misc::TauntFollowsCamera);
			LOAD_VAR(Vars::Misc::BypassPure);
			LOAD_VAR(Vars::Misc::NoisemakerSpam);
			LOAD_VAR(Vars::Misc::DisableInterpolation);
			LOAD_VAR(Vars::Misc::MedalFlip);
			LOAD_VAR(Vars::Misc::AutoRocketJump);
			LOAD_VAR(Vars::Misc::NonLethalRocketJump);
			LOAD_VAR(Vars::Misc::AutoScoutJump);
			LOAD_VAR(Vars::Misc::ChatSpam);
			LOAD_VAR(Vars::Misc::NoPush);
			LOAD_VAR(Vars::Misc::EdgeJump);
			LOAD_VAR(Vars::Misc::EdgeJumpKey);
			LOAD_VAR(Vars::Misc::StoreStatistics);
			LOAD_VAR(Vars::Misc::AutoStrafe);
			LOAD_VAR(Vars::Misc::Directional);
			LOAD_VAR(Vars::Misc::AntiAFK);
			LOAD_VAR(Vars::Misc::CheatsBypass);
			LOAD_VAR(Vars::Misc::RageRetry);
			LOAD_VAR(Vars::Misc::RageRetryHealth);
			LOAD_VAR(Vars::Misc::MVMRes);
			LOAD_VAR(Vars::Misc::VotingOptions);
			LOAD_VAR(Vars::Misc::PingReducer);
			LOAD_VAR(Vars::Misc::PingTarget);
			LOAD_VAR(Vars::Misc::ExtendFreeze);
			LOAD_VAR(Vars::Misc::ViewmodelFlip);
			LOAD_VAR(Vars::Misc::AutoJoin);
			LOAD_VAR(Vars::Misc::KillstreakWeapon);
			LOAD_VAR(Vars::Misc::PartyNetworking);
			LOAD_VAR(Vars::Misc::PartyMarker);
			LOAD_VAR(Vars::Misc::PartyESP);
			LOAD_VAR(Vars::Misc::CrouchSpeed);
			LOAD_VAR(Vars::Misc::FastAccel);
			LOAD_VAR(Vars::Misc::FakeAccelAngle);
			LOAD_VAR(Vars::Misc::SoundBlock);
			LOAD_VAR(Vars::Misc::ChatFlags);
			LOAD_VAR(Vars::Misc::MedievalChat);
			LOAD_VAR(Vars::Misc::AutoAcceptItemDrops);
			LOAD_VAR(Vars::Misc::RegionChanger);
			LOAD_VAR(Vars::Misc::RegionsAllowed);
			LOAD_VAR(Vars::Misc::AutoCasualQueue);
			LOAD_VAR(Vars::Misc::AntiVAC);
			LOAD_VAR(Vars::Misc::InstantAccept);
			LOAD_VAR(Vars::Misc::AntiAutobal);
			LOAD_VAR(Vars::Misc::RunescapeChat);

			// Followbot
			{
				LOAD_VAR(Vars::Misc::Followbot::Enabled);
				LOAD_VAR(Vars::Misc::Followbot::Distance);
			}

			// Cheater Detection
			{
				LOAD_VAR(Vars::Misc::CheaterDetection::Enabled);
				LOAD_VAR(Vars::Misc::CheaterDetection::Methods);
				LOAD_VAR(Vars::Misc::CheaterDetection::Protections);
				LOAD_VAR(Vars::Misc::CheaterDetection::SuspicionGate);
				LOAD_VAR(Vars::Misc::CheaterDetection::PacketManipGate);
				LOAD_VAR(Vars::Misc::CheaterDetection::BHopMaxDelay);
				LOAD_VAR(Vars::Misc::CheaterDetection::BHopDetectionsRequired);
				LOAD_VAR(Vars::Misc::CheaterDetection::ScoreMultiplier);
				LOAD_VAR(Vars::Misc::CheaterDetection::MinimumFlickDistance);
				LOAD_VAR(Vars::Misc::CheaterDetection::MaximumNoise);
				LOAD_VAR(Vars::Misc::CheaterDetection::MinimumAimbotFoV);
				LOAD_VAR(Vars::Misc::CheaterDetection::MaxScaledAimbotFoV);
			}

			// CL_Move
			{
				LOAD_VAR(Vars::Misc::CL_Move::Enabled); //Enabled
				LOAD_VAR(Vars::Misc::CL_Move::Doubletap); // { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::SafeTick);
				LOAD_VAR(Vars::Misc::CL_Move::SafeTickAirOverride);
				LOAD_VAR(Vars::Misc::CL_Move::PassiveRecharge);
				LOAD_VAR(Vars::Misc::CL_Move::WaitForDT); // { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::NotInAir); // { true, L"Doubletap" };
				LOAD_VAR(Vars::Misc::CL_Move::StopMovement);
				LOAD_VAR(Vars::Misc::CL_Move::TeleportKey); // { 0x46, L"Teleport Key" }; //F
				LOAD_VAR(Vars::Misc::CL_Move::TeleportMode);
				LOAD_VAR(Vars::Misc::CL_Move::TeleportFactor);
				LOAD_VAR(Vars::Misc::CL_Move::RechargeKey); // { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::RetainFakelag);
				LOAD_VAR(Vars::Misc::CL_Move::RechargeWhileDead);
				LOAD_VAR(Vars::Misc::CL_Move::AutoRecharge);
				LOAD_VAR(Vars::Misc::CL_Move::AntiWarp);
				LOAD_VAR(Vars::Misc::CL_Move::DoubletapKey); // { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::DTBarStyle);
				LOAD_OTHER(Vars::Misc::CL_Move::DTIndicator);
				LOAD_VAR(Vars::Misc::CL_Move::DTMode);
				LOAD_VAR(Vars::Misc::CL_Move::Fakelag); // { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::FakelagMin);
				LOAD_VAR(Vars::Misc::CL_Move::FakelagMax);
				LOAD_VAR(Vars::Misc::CL_Move::FakelagMode);
				LOAD_VAR(Vars::Misc::CL_Move::WhileMoving);
				LOAD_VAR(Vars::Misc::CL_Move::WhileInAir);
				LOAD_VAR(Vars::Misc::CL_Move::WhileUnducking);
				LOAD_VAR(Vars::Misc::CL_Move::WhileVisible);
				LOAD_VAR(Vars::Misc::CL_Move::PredictVisibility);
				LOAD_VAR(Vars::Misc::CL_Move::FakelagOnKey); // { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::FakelagKey); // { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::FakelagValue); // { 0x52, L"Recharge Key" }; //R
				LOAD_VAR(Vars::Misc::CL_Move::DTTicks);
				LOAD_VAR(Vars::Misc::CL_Move::AutoPeekKey);
				LOAD_VAR(Vars::Misc::CL_Move::AutoPeekDistance);
				LOAD_VAR(Vars::Misc::CL_Move::AutoPeekFree);
				{
					LOAD_VAR(Vars::Misc::CL_Move::FLGChams::Material);
					LOAD_OTHER(Vars::Misc::CL_Move::FLGChams::FakelagColor);
				}
			}
			//Discord
			{
				LOAD_VAR(Vars::Misc::Discord::EnableRPC);
				LOAD_VAR(Vars::Misc::Discord::IncludeMap);
				LOAD_VAR(Vars::Misc::Discord::IncludeClass);
				LOAD_VAR(Vars::Misc::Discord::IncludeTimestamp);
				LOAD_VAR(Vars::Misc::Discord::WhatImagesShouldBeUsed);
			}
			//Steam
			{
				LOAD_VAR(Vars::Misc::Steam::EnableRPC);
				LOAD_VAR(Vars::Misc::Steam::MatchGroup);
				LOAD_VAR(Vars::Misc::Steam::OverrideMenu);
				LOAD_VAR(Vars::Misc::Steam::MapText);
				LOAD_VAR(Vars::Misc::Steam::GroupSize);
				LOAD_VAR(Vars::Misc::Steam::CustomText);
			}
		}

		//AntiHack
		{
			//AntiAim
			{
				LOAD_VAR(Vars::AntiHack::AntiAim::Active);
				LOAD_VAR(Vars::AntiHack::AntiAim::ToggleKey);
				LOAD_VAR(Vars::AntiHack::AntiAim::InvertKey);
				LOAD_VAR(Vars::AntiHack::AntiAim::ManualKey);
				LOAD_VAR(Vars::AntiHack::AntiAim::Pitch);
				LOAD_VAR(Vars::AntiHack::AntiAim::BaseYawMode);
				LOAD_VAR(Vars::AntiHack::AntiAim::YawReal);
				LOAD_VAR(Vars::AntiHack::AntiAim::YawFake);
				LOAD_VAR(Vars::AntiHack::AntiAim::SpinSpeed);
				LOAD_VAR(Vars::AntiHack::AntiAim::CustomRealPitch);
				LOAD_VAR(Vars::AntiHack::AntiAim::BaseYawOffset);
				LOAD_VAR(Vars::AntiHack::AntiAim::CustomRealYaw);
				LOAD_VAR(Vars::AntiHack::AntiAim::CustomFakeYaw);
				LOAD_VAR(Vars::AntiHack::AntiAim::RealJitter);
				LOAD_VAR(Vars::AntiHack::AntiAim::FakeJitter);
				LOAD_VAR(Vars::AntiHack::AntiAim::RandInterval);
				LOAD_VAR(Vars::AntiHack::AntiAim::AntiOverlap);
				LOAD_VAR(Vars::AntiHack::AntiAim::AntiBackstab);
				LOAD_VAR(Vars::AntiHack::AntiAim::LegJitter);
				LOAD_VAR(Vars::AntiHack::AntiAim::InvalidShootPitch);
			}
			//Resolver
			{
				LOAD_VAR(Vars::AntiHack::Resolver::Resolver);
				LOAD_VAR(Vars::AntiHack::Resolver::IgnoreAirborne);
			}
		}

		//Others
		{
			LOAD_OTHER(Vars::Menu::Colors::MenuAccent);

			LOAD_OTHER(Colors::OutlineESP);
			LOAD_OTHER(Colors::DTBarIndicatorsCharged);
			LOAD_OTHER(Colors::DTBarIndicatorsCharging);
			LOAD_OTHER(Colors::ChokedBar);
			LOAD_OTHER(Colors::GradientHealthBar);
			LOAD_OTHER(Colors::GradientOverhealBar);
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
			LOAD_OTHER(Colors::Enemy);
			LOAD_OTHER(Colors::rTeam);
			LOAD_OTHER(Colors::TeamRed);
			LOAD_OTHER(Colors::TeamBlu);
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
			LOAD_OTHER(Colors::FresnelBaseHands);
			LOAD_OTHER(Colors::FresnelBaseWeps);
			LOAD_OTHER(Colors::FresnelTop);
			LOAD_OTHER(Colors::AimSquareCol);
			LOAD_OTHER(Colors::DtOutline);
			LOAD_OTHER(Colors::NotifBG);
			LOAD_OTHER(Colors::NotifOutline);
			LOAD_OTHER(Colors::NotifText);
			LOAD_OTHER(Colors::WeaponIcon);
			LOAD_OTHER(Colors::NoscopeLines1);
			LOAD_OTHER(Colors::NoscopeLines2);
			LOAD_OTHER(Colors::bonecolor);
			LOAD_OTHER(Colors::HitboxFace);
			LOAD_OTHER(Colors::HitboxEdge);

			LOAD_OTHER(Vars::Chams::Players::Local);
			LOAD_OTHER(Vars::Chams::Players::FakeAng);
			LOAD_OTHER(Vars::Chams::Players::Enemy);
			LOAD_OTHER(Vars::Chams::Players::Team);
			LOAD_OTHER(Vars::Chams::Players::Friend);
			LOAD_OTHER(Vars::Chams::Players::Target);
			LOAD_OTHER(Vars::Chams::Players::Ragdoll);
			LOAD_OTHER(Vars::Chams::DME::Hands);
			LOAD_OTHER(Vars::Chams::DME::Weapon);

			LOAD_OTHER(Vars::Chams::Buildings::Local);
			LOAD_OTHER(Vars::Chams::Buildings::Enemy);
			LOAD_OTHER(Vars::Chams::Buildings::Team);
			LOAD_OTHER(Vars::Chams::Buildings::Friend);
			LOAD_OTHER(Vars::Chams::Buildings::Target);

			LOAD_OTHER(Vars::Chams::World::Health);
			LOAD_OTHER(Vars::Chams::World::Ammo);
			LOAD_OTHER(Vars::Chams::World::Projectiles);

			LOAD_OTHER(Vars::Skybox::SkyboxNum);
			LOAD_STRING(Vars::Skybox::SkyboxName);

			LOAD_OTHER(Vars::Menu::ModernDesign);
			LOAD_OTHER(Vars::Menu::ShowPlayerlist);
			LOAD_OTHER(Vars::Menu::ShowKeybinds);
		}

		//Fonts
		{
			//

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

		g_Draw.RemakeFonts
		({
			{0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.Value, Vars::Fonts::FONT_ESP::nWeight.Value, Vars::Fonts::FONT_ESP::nFlags.Value},
			{0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.Value, Vars::Fonts::FONT_ESP_NAME::nWeight.Value, Vars::Fonts::FONT_ESP_NAME::nFlags.Value},
			{0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.Value, Vars::Fonts::FONT_ESP_COND::nWeight.Value, Vars::Fonts::FONT_ESP_COND::nFlags.Value},
			{0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value},
			{0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.Value, Vars::Fonts::FONT_MENU::nWeight.Value, Vars::Fonts::FONT_MENU::nFlags.Value},
			{0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.Value, Vars::Fonts::FONT_INDICATORS::nWeight.Value, Vars::Fonts::FONT_INDICATORS::nFlags.Value},
			{0x0, "Verdana", 18, 800, FONTFLAG_ANTIALIAS},
			{ 0x0, "Verdana", 12, 800, FONTFLAG_DROPSHADOW},
		 });

		CurrentConfig = configName;
		F::Notifications.Add("Config " + configName + " loaded");
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void CConfigManager::RemoveConfig(const std::string& configName)
{
	std::filesystem::remove(ConfigPath + "\\" + configName + ConfigExtension);
}
