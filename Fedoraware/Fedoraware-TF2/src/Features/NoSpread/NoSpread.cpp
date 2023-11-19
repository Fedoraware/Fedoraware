#include "NoSpread.h"
#include <regex>
#define VALVE_RAND_MAX 0x7FFF

// Credits to Nullworks/Cathook team ( projectile nospread )
// and spook ( hitscan nospread )

void CNoSpread::CreateMoveProjectile(CUserCmd* pCmd) 
{
    const auto pLocal = g_EntityCache.GetLocal();
    const auto pWeapon = g_EntityCache.GetWeapon();
    if (!pLocal || !pWeapon)
        return;

    if (!pLocal->IsAlive())
        return;

    if (!Vars::NoSpread::Projectile.Value)
        return;

    // Credits to https://www.unknowncheats.me/forum/team-fortress-2-a/139094-projectile-nospread.html
 
    // Projectile check
    if (G::CurWeaponType != EWeaponType::PROJECTILE)
        return;

    // Set up Random Seed
    int iCmdNum = pCmd->command_number;
    Utils::RandomSeed(MD5_PseudoRandom(iCmdNum) & 0x7FFFFFFF);
    Utils::SharedRandomInt(MD5_PseudoRandom(iCmdNum) & 0x7FFFFFFF, "SelectWeightedSequence", 0, 0, 0);
    for (int i = 0; i < 6; ++i)
       Utils::RandomFloat(-1, 1);

    // Beggars check
    if (pWeapon->GetItemDefIndex() == Soldier_m_TheBeggarsBazooka)
    {
        // Player has 0 loaded rockets and reload mode is not 2 (reloading and ready to release)
        bool bNoRockets = pWeapon->GetClip1() == 0 && pWeapon->GetReloadMode() != 2;
        // Player is attacking and reload is not 0 (not reloading)
        bool bLoading = pCmd->buttons & IN_ATTACK && pWeapon->GetReloadMode() != 0;
        if (bNoRockets || bLoading)
            return;
    }
    // Huntsman check
    else if (pWeapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW)
    {
        if (pCmd->buttons & IN_ATTACK || pWeapon->GetChargeBeginTime() == 0)
            return;
    }
    // Rest of weapons
    else if (!(pCmd->buttons & IN_ATTACK))
        return;

    switch (pWeapon->GetWeaponID())
    {
    case TF_WEAPON_SYRINGEGUN_MEDIC:
    {
        float flSpread = 1.5f;
        pCmd->viewangles.x -= Utils::RandomFloat(-flSpread, flSpread);
        pCmd->viewangles.y -= Utils::RandomFloat(-flSpread, flSpread);
        Math::ClampAngles(pCmd->viewangles);
        break;
    }
    case TF_WEAPON_COMPOUND_BOW:
    {
        Vec3 vView = I::EngineClient->GetViewAngles();
        Vec3 vSpread;
        Vec3 vSrc;

        pWeapon->GetProjectileFireSetup(pLocal, Vec3(23.5f, 8.0f, -3.0f), &vSrc, &vSpread, false, 2000.0f);
        float flTotalChargeTime = I::GlobalVars->curtime - pWeapon->GetChargeBeginTime();
        if (flTotalChargeTime >= 5.0f)
        {
            float flRand = (float)Utils::RandomInt() / VALVE_RAND_MAX;
            vSpread.x += -6 + flRand * 12.f;
            flRand = (float)Utils::RandomInt() / VALVE_RAND_MAX;
            vSpread.y += -6 + flRand * 12.f;
        }
        vSpread -= vView;
        pCmd->viewangles -= vSpread;
        Math::ClampAngles(pCmd->viewangles);
        break;
    }
    default:
        Vec3 vView = I::EngineClient->GetViewAngles();
        Vec3 vSpread;

        pWeapon->GetSpreadAngles(vSpread);
        vSpread -= vView;
        pCmd->viewangles -= vSpread;
        Math::ClampAngles(pCmd->viewangles);
        break;
    }
    G::SilentTime = true;
}


void CNoSpread::AskForPlayerPerf()
{
	if (!Vars::NoSpread::Hitscan.Value)
	{
		Reset();

		return;
	}

	CBaseCombatWeapon* weapon = g_EntityCache.GetWeapon();

	// Commented out so it doesnt resync when you switch to non-hitscan weapons
	if (!weapon /* || !(weapon->GetDamageType() & DMG_BULLET) || Utils::GetWeaponType(weapon) != EWeaponType::HITSCAN || weapon->GetWeaponSpread() <= 0.0f*/)
	{
		Reset();

		return;
	}

	if (CBaseEntity * local = g_EntityCache.GetLocal())
	{
		if (local->deadflag())
		{
			Reset();

			return;
		}
	}

	if (WaitingForPP)
	{
		return;
	}

	I::EngineClient->ClientCmd_Unrestricted("playerperf");

	AskTime = static_cast<float>(Utils::PlatFloatTime());

	WaitingForPP = true;
}

bool CNoSpread::ParsePlayerPerf(bf_read& msg_data)
{
	if (!Vars::NoSpread::Hitscan.Value)
	{
		return false;
	}

	char raw_msg[256]{};

	msg_data.ReadString(raw_msg, sizeof(raw_msg), true);
	msg_data.Seek(0);

	std::string msg(raw_msg);

	msg.erase(msg.begin()); // STX

	std::smatch matches{};

	std::regex_match(msg, matches, std::regex("(\\d+.\\d+)\\s\\d+\\s\\d+\\s\\d+.\\d+\\s\\d+.\\d+\\svel\\s\\d+.\\d+"));

	if (matches.size() == 2)
	{
		WaitingForPP = false;

		// credits to kgb for idea

		float new_server_time{ std::stof(matches[1].str()) };

		if (new_server_time > ServerTime)
		{
			PrevServerTime = ServerTime;

			ServerTime = new_server_time;

			ResponseTime = static_cast<float>(Utils::PlatFloatTime() - AskTime);

			if (PrevServerTime > 0.0f)
			{
				if (GuessTime > 0.0f)
				{
					float delta{ ServerTime - GuessTime };

					if (delta == 0.0f)
					{
						Synced = true;

						SyncOffset = GuessDelta;
					}
				}

				GuessDelta = ServerTime - PrevServerTime;

				GuessTime = ServerTime + (GuessDelta);
			}
		}

		return true;
	}

	else
	{
		return std::regex_match(msg, std::regex("\\d+.\\d+\\s\\d+\\s\\d+"));
	}

	return false;
}

int CNoSpread::GetSeed()
{
	float time{ (ServerTime + SyncOffset + ResponseTime) * 1000.0f };

	return *reinterpret_cast<int*>((char*)&time) & 255;
}

void CNoSpread::Reset()
{
	Synced = false;
	ServerTime = 0.0f;
	PrevServerTime = 0.0f;
	AskTime = 0.0f;
	GuessTime = 0.0f;
	SyncOffset = 0.0f;
	WaitingForPP = false;
	GuessDelta = 0.0f;
	ResponseTime = 0.0f;
}

void CNoSpread::CreateMoveHitscan(CUserCmd* cmd)
{
	if (!Vars::NoSpread::Hitscan.Value || !Synced || !cmd)
	{
		return;
	}

	auto local = g_EntityCache.GetLocal();

	if (!local)
	{
		return;
	}

	auto weapon = g_EntityCache.GetWeapon();

	if (!weapon || !(weapon->GetDamageType() & DMG_BULLET) || !Utils::IsAttacking(cmd, weapon))
	{
		return;
	}

	auto spread = weapon->GetWeaponSpread();

	if (spread <= 0.0f)
	{
		return;
	}

	auto bullets_per_shot = weapon->GetWeaponData().m_nBulletsPerShot;

	bullets_per_shot = static_cast<int>(Utils::ATTRIB_HOOK_FLOAT(bullets_per_shot, "mult_bullets_per_shot", weapon, 0, true));

	// credits to cathook for average spread stuff

	std::vector<Vec3> bullet_corrections{};

	Vec3 average_spread{};

	auto seed = GetSeed();

	for (auto bullet{ 0 }; bullet < bullets_per_shot; bullet++)
	{
		Utils::RandomSeed(seed++);

		auto fire_perfect{ false };

		if (bullet == 0)
		{
			auto time_since_last_shot{ (local->m_nTickBase() * TICK_INTERVAL) - weapon->GetLastFireTime() };

			if (bullets_per_shot > 1 && time_since_last_shot > 0.25f)
			{
				fire_perfect = true;
			}

			else if (bullets_per_shot == 1 && time_since_last_shot > 1.25f)
			{
				fire_perfect = true;
			}
		}

		if (fire_perfect)
		{
			return;
		}

		auto x{ Utils::RandomFloat(-0.5f, 0.5f) + Utils::RandomFloat(-0.5f, 0.5f) };
		auto y{ Utils::RandomFloat(-0.5f, 0.5f) + Utils::RandomFloat(-0.5f, 0.5f) };

		Vec3 forward{}, right{}, up{};

		Math::AngleVectors(cmd->viewangles, &forward, &right, &up);

		Vector fixed_spread{ forward + (right * x * spread) + (up * y * spread) };

		fixed_spread.NormalizeInPlace();

		average_spread += fixed_spread;

		bullet_corrections.push_back(fixed_spread);
	}

	average_spread /= static_cast<float>(bullets_per_shot);

	Vec3 fixed_spread{ FLT_MAX, FLT_MAX, FLT_MAX };

	for (const auto& spread : bullet_corrections)
	{
		if (spread.DistTo(average_spread) < fixed_spread.DistTo(average_spread))
		{
			fixed_spread = spread;
		}
	}

	Vec3 fixed_angles{};

	Math::VectorAngles(fixed_spread, fixed_angles);

	Vec3 correction{ cmd->viewangles - fixed_angles };

	cmd->viewangles += correction;

	Math::ClampAngles(cmd->viewangles);

	G::SilentTime = true;
}