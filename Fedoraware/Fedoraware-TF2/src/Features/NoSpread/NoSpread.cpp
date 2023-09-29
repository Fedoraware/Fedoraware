#include "NoSpread.h"
#include <regex>
#include <boost/algorithm/string.hpp>
#define VALVE_RAND_MAX 0x7FFF

// Credits to Nullworks/Cathook team :))
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


float CNoSpread::CalcMantissaStep(float flValue)
{
    int iRawValue = reinterpret_cast<int&>(flValue);
    int iExponent = (iRawValue >> 23) & 0xFF;
    return powf(2, iExponent - (127 + 23));
}

float CNoSpread::ServerCurTime()
{
    const auto pLocal = g_EntityCache.GetLocal();
    if (!pLocal)
        return 0.f;

    float flServerTime = TICK_INTERVAL * pLocal->m_nTickBase();
    return flServerTime;
}

void CNoSpread::Reset()
{ 
    G::NoSpreadSynced = NOT_SYNCED;
    G::BadMantissa = false;
    bWaitingPerfData = false;
    bWaitingForPostSNM = false;
    bResyncNeeded = false;
    bLastWasPlayerPerf = false;
    bShouldUpdateTime = true;
    bFirstUserCmd = false;
    bShouldUpdateUserCmdCorrection = false;
    bResyncedThisDeath = false;
    bIsSyncing = false;

    flPredictionSeed = 0.0f;
    flMantissaStep = 0.0f;
    flCurrentWeaponSpread = 0.0f;

    G::SentClientFloatTime = 0.0;
    dFloatTimeDelta = 0.0;
    dLastSyncDeltaTime = 0.0;
    dServerTime = 0.0;
    dTimeStart = 0.0;
    dWriteUserCmdCorrection = 0.0;
    dPingAtSend = 0.0;
    dLastCorrection = 0.0;
    dLastPingAtSend = 0.0;

    iNewPackets = 0;

    UserCmdBackup = {};
}

bool CNoSpread::IsPerfectShot(CBaseCombatWeapon* weapon, float flProvidedTime)
{
    float flServerTime = flProvidedTime == 0.0 ? ServerCurTime() : flProvidedTime;
    float flTimeSinceAttack = flServerTime - weapon->GetLastFireTime();
    WeaponData_t wdata = weapon->GetWeaponData();
    int nBulletsPerShot = wdata.m_nBulletsPerShot;
    if (nBulletsPerShot >= 1)
        nBulletsPerShot = Utils::ATTRIB_HOOK_FLOAT(nBulletsPerShot, "mult_bullets_per_shot", weapon, nullptr, true);
    else
        nBulletsPerShot = 1;
    if ((nBulletsPerShot == 1 && flTimeSinceAttack > 1.25f) || (nBulletsPerShot > 1 && flTimeSinceAttack > 0.25f))
        return true;
    return false;
}

bool CNoSpread::SendNetMessage(INetMessage* data)
{
    if (!Vars::NoSpread::Hitscan.Value)
        return false;

    // if we send clc_move with playerperf command or corrected angles, we must ensure it will be sent via reliable stream
    if (bShouldUpdateTime)
    {
        if (data->GetType() != clc_Move)
            return false;

        // and wait for post call
        bWaitingForPostSNM = true;

        // Force reliable
        return true;
    }
    else if (G::NoSpreadSynced)
    {
        if (data->GetType() != clc_Move)
            return false;
    }
    return false;
}

void CNoSpread::SendNetMessagePost()
{
    static Timer WaitPerf;
    if (!bWaitingForPostSNM || !Vars::NoSpread::Hitscan.Value || (bWaitingPerfData && !WaitPerf.Run(1000)))
        return;

    bWaitingForPostSNM = false;

    // Create playerperf
    NET_StringCmd sCmd("playerperf");

    // And send it along with our clc_move. Yes, we are calling SendNetMsg from inside SendNetMsg
   I::EngineClient->GetNetChannelInfo()->SendNetMsg(sCmd, true);

    // remember client float time
    bShouldUpdateTime = false;
    // Only set when not syncing
    if (G::NoSpreadSynced == NOT_SYNCED)
        G::SentClientFloatTime = Utils::PlatFloatTime();

    // force transmit now
     I::EngineClient->GetNetChannelInfo()->Transmit();

    if (Vars::NoSpread::UseAvgLatency.Value)
        dPingAtSend = I::EngineClient->GetNetChannelInfo()->GetAvgLatency(FLOW_OUTGOING);
    else
        dPingAtSend = I::EngineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);

    bWaitingPerfData = true;
    WaitPerf.Update();
}

bool  CNoSpread::DispatchUserMessage(bf_read* buf, int type)
{
    bool should_call_original = true;
    if ((!bWaitingPerfData && !bLastWasPlayerPerf) || !Vars::NoSpread::Hitscan.Value)
        return should_call_original;

    // We are looking for TextMsg
    if (type != 5)
        return should_call_original;

    int message_dest = buf->ReadByte();

    // Not send to us
    if (message_dest != 2)
    {
        buf->Seek(0);
        return should_call_original;
    }

    double start_time = Utils::PlatFloatTime();

    char msg_str[256];
    buf->ReadString(msg_str, sizeof(msg_str));
    buf->Seek(0);

    std::vector<std::string> lines;
    boost::split(lines, msg_str, boost::is_any_of("\n"), boost::token_compress_on);

    // Regex to find the playerperf data we want/need
    static std::regex primary_regex("^(([0-9]+\\.[0-9]+) ([0-9]{1,2}) ([0-9]{1,2}))$");

    std::vector<double> vData;

    for (const auto& sStr : lines)
    {
        std::smatch sMatch;

        if (!std::regex_match(sStr, sMatch, primary_regex) || sMatch.size() != 5)
        {
            static std::regex backup_regex(R"(^(([0-9]+\.[0-9]+) ([0-9]{1,2}) ([0-9]{1,2}) ([0-9]+\.[0-9]+) ([0-9]+\.[0-9]+) vel ([0-9]+\.[0-9]+))$)");
            std::smatch sMatch2;
            if (std::regex_match(sStr, sMatch2, backup_regex) && sMatch2.size() > 5)
            {
                bLastWasPlayerPerf = true;
                should_call_original = false;
            }
            continue;
        }

        std::string server_time = sMatch[2].str();

        char* tmp;
        try
        {
            vData.push_back(std::strtod(server_time.c_str(), &tmp));
        }
        // Shouldn't happen
        catch (const std::invalid_argument&)
        {
        }
    }

    if (vData.size() < 2)
    {
        if (vData.empty())
            bLastWasPlayerPerf = false;
        // Still do not call original, we don't want the playerperf spewing everywhere
        else
            return false;
        return should_call_original; // not our case, just return
    }

    // Less than 1 in step size is literally impossible to predict, although 1 is already pushing it
    if (CalcMantissaStep(vData[0] * 1000.0) < 1.0)
    {
        if (bWaitingPerfData)
        bWaitingPerfData = false;
        bLastWasPlayerPerf = true;
        should_call_original = false;
        G::BadMantissa = true;
        G::NoSpreadSynced = NOT_SYNCED;
        return should_call_original;
    }

    G::BadMantissa = false;
    bLastWasPlayerPerf = true;
    should_call_original = false;

    // Process time!

    // ...or not
    if (!bWaitingPerfData)
        return should_call_original;

    if (G::NoSpreadSynced == NOT_SYNCED)
    {
        // first, compensate procession time and latency between us and server
        double client_time = Utils::PlatFloatTime();
        double total_latency = (client_time - (client_time - start_time)) - G::SentClientFloatTime;

        // Second compensate latency and get delta time (this might be negative, so be careful!)
        dFloatTimeDelta = vData[0] - G::SentClientFloatTime;

        // We got time with latency included, but only outgoing, so compensate
        dFloatTimeDelta -= (total_latency / 2.0);

        // we need only first output which is latest
        bWaitingPerfData = false;

        // and now collect history
        G::NoSpreadSynced = CORRECTING;
        bIsSyncing = true;
    }
    else if (G::NoSpreadSynced != SYNCED)
    {
        // Now sync and Correct
        double time_difference = G::SentClientFloatTime - vData[0];

        double mantissa_step = CalcMantissaStep(G::SentClientFloatTime * 1000.0);
        // Apply correction
        // We must try to be super accurate if the rvar is set, else base on mantissa step
        double correction_threshhold = Vars::NoSpread::ExtremePred.Value ? 0.001 : (mantissa_step / 1000.0 / 2.0);

        // Check if we are not precise enough or snapped too hard for it to actually be synced
        if (abs(time_difference) > correction_threshhold || abs(dLastCorrection) > mantissa_step / 1000.0)
        {
            dFloatTimeDelta -= time_difference;
            // it will auto resync it
            bResyncNeeded = true;
        }
        // We synced successfully
        else
        {
            bResyncNeeded = false;
        }
        dLastCorrection = time_difference;

        // We need only first output which is latest
        bWaitingPerfData = false;

        // We are synced.
        if (!bResyncNeeded)
            G::NoSpreadSynced = SYNCED;
    }
    // May happen when dead
    else
    {
        bResyncNeeded = false;
        bWaitingPerfData = false;
    }
    return should_call_original;
}

void CNoSpread::ClSendMove()
{
    bFirstUserCmd = true;

    if (!G::NoSpreadSynced || !Vars::NoSpread::Hitscan.Value || G::ShouldShift)
        return;

    flCurrentWeaponSpread = 0.0f;

    const auto pLocal = g_EntityCache.GetLocal();

    if (!pLocal)
        return;

    const auto pWeapon = g_EntityCache.GetWeapon();

    int new_packets = 1 + I::ClientState->chokedcommands;

    double dAsumedRealTime = Utils::PlatFloatTime() + dFloatTimeDelta;
    double dPredictedTime = dAsumedRealTime;

    dPredictedTime += dWriteUserCmdCorrection * new_packets;

    double dPing = I::ClientState->m_NetChannel->GetLatency(FLOW_OUTGOING);
    if (Vars::NoSpread::UseAvgLatency.Value)
        dPing = I::ClientState->m_NetChannel->GetAvgLatency(FLOW_OUTGOING);

    if (Vars::NoSpread::CorrectPing.Value)
        if (!(G::IsChoking) && (int)(dPing * 1000.0) != (int)(dPingAtSend * 1000.0))
            dPredictedTime += dPing - dPingAtSend;

    static Timer s_NextCheck;

    if (s_NextCheck.Check(1000) && pLocal->IsAlive() && !bWaitingPerfData) {
        s_NextCheck.Update();

        G::SentClientFloatTime = dAsumedRealTime;
        bShouldUpdateTime = true;

        if (!pLocal->IsAlive() && G::NoSpreadSynced != CORRECTING) {
            dLastSyncDeltaTime = dFloatTimeDelta;
            dLastPingAtSend = dPingAtSend;
            G::NoSpreadSynced = DEAD_SYNC;
        }
        bResyncedThisDeath = true;
    }

    if (!pLocal->IsAlive())
        return;
    else {
        bResyncedThisDeath = false;

        if (G::NoSpreadSynced == DEAD_SYNC)
        {
            dFloatTimeDelta = dLastSyncDeltaTime;
            dPingAtSend = dLastPingAtSend;
            G::NoSpreadSynced = SYNCED;
            bShouldUpdateTime = false;
        }
    }

    if (pWeapon && !pWeapon->IsSpreadWeapon())
        return;

    float flCurTime = ServerCurTime();

    if (!(G::LastUserCmd->buttons & IN_ATTACK))
        return;

    if (pWeapon) {
        flCurrentWeaponSpread = pWeapon->GetWeaponSpread();
    }

    if (!std::isfinite(flCurrentWeaponSpread)) {
        return;
    }

    flPredictionSeed = (float)(dPredictedTime * 1000.0f);
    dTimeStart = Utils::PlatFloatTime();
    iNewPackets = new_packets;
}

void CNoSpread::ClSendMovePost() {

    if (!Vars::NoSpread::Hitscan.Value || G::ShouldShift)
        return;

    const auto pLocal = g_EntityCache.GetLocal();
    const auto pWeapon = g_EntityCache.GetWeapon();

    if (!pLocal || !pWeapon)
        return;

    if (!pWeapon->IsSpreadWeapon() || !pLocal->IsAlive())
        return;

    double time_end = Utils::PlatFloatTime();

    if (bShouldUpdateUserCmdCorrection) {
        dWriteUserCmdCorrection = (time_end - dTimeStart) / iNewPackets;
        bShouldUpdateUserCmdCorrection = false;
    }
}

void CNoSpread::CreateMoveHitscan(CUserCmd* pCmd) {
    static Timer UpdateNospread;
    if (G::ShouldShift)
        return;

    if (G::NoSpreadSynced == SYNCED)
        bIsSyncing = false;

    const auto pLocal = g_EntityCache.GetLocal();
    const auto pWeapon = g_EntityCache.GetWeapon();

    if (!pLocal || !pWeapon)
        return;

    if (!pLocal->IsAlive() || G::CurWeaponType == EWeaponType::PROJECTILE || G::CurWeaponType == EWeaponType::MELEE)
        return;

    if (G::NoSpreadSynced == NOT_SYNCED && !G::BadMantissa)
    {
        bIsSyncing = true;
        bShouldUpdateTime = true;
        UpdateNospread.Update();
    }
    else if (G::NoSpreadSynced == NOT_SYNCED && UpdateNospread.Run(1000))
        G::NoSpreadSynced = CORRECTING;

    if ((G::NoSpreadSynced != SYNCED && !bResyncNeeded) || flCurrentWeaponSpread == 0.0 || !Vars::NoSpread::Hitscan.Value)
        return;

    if (!(pCmd->buttons & IN_ATTACK)) {
        UserCmdBackup = *pCmd;
        bFirstUserCmd = false;
        return;
    }

    ApplySpreadCorrection(pCmd->viewangles, reinterpret_cast<int&>(flPredictionSeed) & 0xFF, flCurrentWeaponSpread);
    bShouldUpdateUserCmdCorrection = true;

    UserCmdBackup = *pCmd;
    bFirstUserCmd = false;
}

void CNoSpread::ApplySpreadCorrection(Vec3& vAngles, int iSeed, float flSpread) {
    const auto pLocal = g_EntityCache.GetLocal();
    const auto pWeapon = g_EntityCache.GetWeapon();

    if (!pLocal || !pWeapon)
        return;

    if (!pLocal->IsAlive())
        return;

    bool bIsFirstShotPerfect = IsPerfectShot(pWeapon);

    int nBulletsPerShot = pWeapon->GetWeaponData().m_nBulletsPerShot;

    if (nBulletsPerShot >= 1)
        nBulletsPerShot = Utils::ATTRIB_HOOK_FLOAT(nBulletsPerShot, "mult_bullets_per_shot", pWeapon, 0x0, true);

    else nBulletsPerShot = 1;


    std::vector< Vec3 > BulletCorrections;
    Vec3 vAverageSpread(0, 0, 0);

    for (int i = 0; i < nBulletsPerShot; i++) {
        Utils::RandomSeed(iSeed + i);
        auto x = Utils::RandomFloat(-0.5, 0.5) + Utils::RandomFloat(-0.5, 0.5);
        auto y = Utils::RandomFloat(-0.5, 0.5) + Utils::RandomFloat(-0.5, 0.5);

        if (bIsFirstShotPerfect && !i) {
            x = 0.0f;
            y = 0.0f;
        }

        Math::ClampAngles(vAngles);
        auto vForward = Vec3(), vRight = Vec3(), vUp = Vec3();
        Math::AngleVectors(vAngles, &vForward, &vRight, &vUp);

        Vec3 vFixedSpread = vForward + (vRight * x * flSpread) + (vUp * y * flSpread);
        vFixedSpread.NormalizeInPlace();
        vAverageSpread += vFixedSpread;
        BulletCorrections.push_back(vFixedSpread);
    }

    vAverageSpread /= nBulletsPerShot;

    Vec3 vFixedSpread(FLT_MAX, FLT_MAX, FLT_MAX);

    for (auto& vSpread : BulletCorrections) {
        if (vSpread.DistTo(vAverageSpread) < vFixedSpread.DistTo(vAverageSpread))
            vFixedSpread = vSpread;
    }

    Vec3 vFixedAng;
    Math::VectorAngles(vFixedSpread, vFixedAng);
    Vec3 vCorrection = (vAngles - vFixedAng);
    vAngles += vCorrection;
    Math::ClampAngles(vAngles);
    G::SilentTime = true;
}