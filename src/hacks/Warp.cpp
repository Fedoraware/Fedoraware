/*
 * Created on 16.4.2020
 * Author: BenCat07
 *
 * Copyright Nullworks 2020
 */

#include "common.hpp"
#if ENABLE_VISUALS
#include "drawing.hpp"
#endif
#include "MiscAimbot.hpp"
#include "PlayerTools.hpp"
#include "DetourHook.hpp"
#include "WeaponData.hpp"
#include "MiscTemporary.hpp"
#include "Think.hpp"
#include "Aimbot.hpp"

namespace hacks::tf2::warp
{
static settings::Boolean enabled{ "warp.enabled", "false" };
static settings::Boolean no_movement{ "warp.rapidfire.no-movement", "true" };
static settings::Boolean rapidfire{ "warp.rapidfire", "false" };
static settings::Int distance{ "warp.rapidfire.distance", "0" };
static settings::Boolean rapidfire_zoom{ "warp.rapidfire.zoom", "true" };
static settings::Boolean wait_full{ "warp.rapidfire.wait-full", "true" };
static settings::Button rapidfire_key{ "warp.rapidfire.key", "<null>" };
static settings::Int rapidfire_key_mode{ "warp.rapidfire.key-mode", "1" };
static settings::Int rf_disable_on{ "warp.rapidfire.disable-on", "0" };
static settings::Float speed{ "warp.speed", "23" };
static settings::Boolean draw{ "warp.draw", "false" };
static settings::Boolean draw_bar{ "warp.draw-bar", "false" };
static settings::Button warp_key{ "warp.key", "<null>" };
static settings::Button charge_key{ "warp.charge-key", "<null>" };
static settings::Boolean charge_passively{ "warp.charge-passively", "true" };
static settings::Boolean charge_in_jump{ "warp.charge-passively.jump", "true" };
static settings::Boolean charge_no_input{ "warp.charge-passively.no-inputs", "false" };
static settings::Int warp_movement_ratio{ "warp.movement-ratio", "6" };
static settings::Boolean warp_demoknight{ "warp.demoknight", "false" };
static settings::Boolean warp_peek{ "warp.peek", "false" };
static settings::Boolean warp_on_damage{ "warp.on-hit", "false" };
static settings::Boolean warp_forward{ "warp.on-hit.forward", "false" };
static settings::Boolean warp_backwards{ "warp.on-hit.backwards", "false" };
static settings::Boolean warp_left{ "warp.on-hit.left", "true" };
static settings::Boolean warp_right{ "warp.on-hit.right", "true" };

static settings::Boolean debug_seqout{ "debug.warp_seqout", "false" };

// Hidden control rvars for communtiy servers
static settings::Int maxusrcmdprocessticks("warp.maxusrcmdprocessticks", "24");

bool in_warp           = false;
bool in_rapidfire      = false;
bool in_rapidfire_zoom = false;
// Should we choke the packet or not? (in rapidfire)
bool choke_packet = false;
// Were we warping last tick?
// why is this needed at all, why do i have to write this janky bs
bool was_in_warp = false;
// Is this the first warp tick?
bool first_warp_tick = false;

// How many ticks we have to add to our CreateMove packet
int ticks_to_add = 0;

int GetMaxWarpTicks();
void warpLogic();

// Draw control
static settings::Int size{ "warp.bar-size", "100" };
static settings::Int bar_x{ "warp.bar-x", "50" };
static settings::Int bar_y{ "warp.bar-y", "200" };
static settings::Int draw_string_x{ "warp.draw-info.x", "8" };
static settings::Int draw_string_y{ "warp.draw-info.y", "800" };

// Need our own Text drawing
static std::array<std::string, 32> warp_strings;
#if ENABLE_VISUALS
static size_t warp_strings_count{ 0 };
static std::array<rgba_t, 32> warp_strings_colors{ colors::empty };

void AddWarpString(const std::string &string, const rgba_t &color)
{
    warp_strings[warp_strings_count]        = string;
    warp_strings_colors[warp_strings_count] = color;
    ++warp_strings_count;
}

void DrawWarpStrings()
{
    float x = *draw_string_x;
    float y = *draw_string_y;
    for (size_t i = 0; i < warp_strings_count; ++i)
    {
        float sx, sy;
        fonts::menu->stringSize(warp_strings[i], &sx, &sy);
        draw::String(x, y, warp_strings_colors[i], warp_strings[i].c_str(), *fonts::center_screen);
        y += fonts::center_screen->size + 1;
    }
    warp_strings_count = 0;
}
#endif

static bool should_charge       = false;
static int warp_amount          = 0;
static int warp_amount_override = 0;
static bool should_melee        = false;
static bool charged             = false;

static bool should_warp = true;
static bool was_hurt    = false;

// Rapidfire key mode
static bool key_valid = false;

// A function that determins whether our key state allows us to rapidfire or not
bool UpdateRFKey()
{
    static bool key_flip          = false;
    static bool pressed_last_tick = false;
    bool allow_key                = true;

    // Check if the key is even used
    if (rapidfire_key && rapidfire_key_mode)
    {
        bool key_down = rapidfire_key.isKeyDown();
        switch ((int) rapidfire_key_mode)
        {
        case 1: // Only while key is pressed
            if (!key_down)
                allow_key = false;
            break;
        case 2: // Only while key is not pressed
            if (key_down)
                allow_key = false;
            break;
        case 3: // Key acts like a toggle switch
            if (!pressed_last_tick && key_down)
                key_flip = !key_flip;
            if (!key_flip)
                allow_key = false;
            break;
        default:
            break;
        }
        pressed_last_tick = key_down;
    }
    // Return whether the key allows it
    return allow_key;
}

float getFireDelay()
{
    auto weapon_data = GetWeaponData(RAW_ENT(LOCAL_W));
    return re::C_TFWeaponBase::ApplyFireDelay(RAW_ENT(LOCAL_W), weapon_data->m_flTimeFireDelay);
}

bool canInstaZoom()
{
    return in_rapidfire_zoom || (g_pLocalPlayer->holding_sniper_rifle && current_user_cmd->buttons & IN_ATTACK2 && !HasCondition<TFCond_Zoomed>(LOCAL_E) && CE_FLOAT(LOCAL_W, netvar.flNextSecondaryAttack) <= g_GlobalVars->curtime);
}

// This is needed in order to make zoom/unzoom smooth even with insta zoom
static int ignore_ticks = 0;

void handleSniper()
{
    // Prevent unzooming
    if (in_rapidfire)
    {
        // Holding ready sniper rifle
        if (canInstaZoom())
        {
            current_user_cmd->buttons &= ~IN_ATTACK2;
            ignore_ticks = TIME_TO_TICKS(0.2f);
        }
    }
    else if (ignore_ticks)
    {
        ignore_ticks--;
        current_user_cmd->buttons &= ~IN_ATTACK2;
    }
}

bool shouldRapidfire()
{
    if (!rapidfire)
        return false;

    // Already in rapidfire
    if (in_rapidfire)
        return false;

    // Only run if key state allows it
    if (!key_valid)
        return false;

    // Dead player
    if (CE_BAD(LOCAL_E) || !LOCAL_E->m_bAlivePlayer() || CE_BAD(LOCAL_W))
        return false;

    // Weapon specific ignores, knives, Thermal Thruster, Huntsman, Mediguns, and the grappling hook
    if (re::C_TFWeaponBase::GetWeaponID(RAW_ENT(LOCAL_W)) == 7 || LOCAL_W->m_iClassID() == CL_CLASS(CTFRocketPack) || LOCAL_W->m_iClassID() == CL_CLASS(CTFCompoundBow) || LOCAL_W->m_iClassID() == CL_CLASS(CWeaponMedigun) || LOCAL_W->m_iClassID() == CL_CLASS(CTFGrapplingHook))
        return false;

    // Ignore throwables/consumables/etc
    if (g_pLocalPlayer->weapon_mode == weapon_throwable || g_pLocalPlayer->weapon_mode == weapon_consumable || g_pLocalPlayer->weapon_mode == weapon_pda)
        return false;

    // Unrevved minigun cannot rapidfire
    if (LOCAL_W->m_iClassID() == CL_CLASS(CTFMinigun) && CE_INT(LOCAL_W, netvar.iWeaponState) != 3 && CE_INT(LOCAL_W, netvar.iWeaponState) != 2)
        return false;

    // Check if enemies are close enough
    if (distance)
    {
        auto closest = getClosestNonlocalEntity(LOCAL_E->m_vecOrigin());
        if (!closest || closest->m_flDistance() > *distance)
            return false;
    }

    // We do not have the amount of ticks needed, don't try it
    if (warp_amount < TIME_TO_TICKS(getFireDelay()) && (TIME_TO_TICKS(getFireDelay()) < *maxusrcmdprocessticks - 1 || (wait_full && warp_amount != GetMaxWarpTicks())))
        return false;

    // Mouse 1 is held, do it.
    bool buttons_pressed = current_user_cmd && current_user_cmd->buttons & IN_ATTACK;

    // Unless we are on a flamethrower, where we want both m1 and m2.
    if (LOCAL_W->m_iClassID() == CL_CLASS(CTFFlameThrower))
        buttons_pressed = current_user_cmd && current_user_cmd->buttons & (IN_ATTACK | IN_ATTACK2);

    if (g_pLocalPlayer->holding_sniper_rifle)
    {
        // Run if m2 is pressed and sniper rifle is ready
        buttons_pressed = rapidfire_zoom && current_user_cmd && current_user_cmd->buttons & IN_ATTACK2 && canInstaZoom();
        // Heatmaker is the only exception here, it can also run on m1
        if (!buttons_pressed && HasCondition<TFCond_FocusBuff>(LOCAL_E))
            buttons_pressed = current_user_cmd && current_user_cmd->buttons & IN_ATTACK;
    }

    switch (*rf_disable_on)
    {
    case 0: // Always on
        return buttons_pressed;
    case 1: // Disable on projectile
        if (g_pLocalPlayer->weapon_mode == weapon_projectile)
            return false;
        break;
    case 2: // Disable on melee
        if (g_pLocalPlayer->weapon_mode == weapon_melee)
            return false;
        break;
    case 3: // Disable on projectile and melee
        if (g_pLocalPlayer->weapon_mode == weapon_projectile || g_pLocalPlayer->weapon_mode == weapon_melee)
            return false;
        break;
    }

    return buttons_pressed;
}

// Should we warp?
bool shouldWarp(bool check_amount)
{
    return
        // Ingame?
        g_IEngine->IsInGame() &&
        // Warp key held?
        (((warp_key && warp_key.isKeyDown())
          // Hurt warp?
          || was_hurt
          // Rapidfire and trying to attack?
          || shouldRapidfire()) &&
         // Do we have enough to warp?
         (!check_amount || warp_amount));
}

// How many ticks of excess we have (for decimal speeds)
float excess_ticks = 0.0f;
int GetWarpAmount(bool finalTick)
{
    int max_extra_ticks = *maxusrcmdprocessticks - 1;

    // Rapidfire ignores speed, and we send 15 + 7, aka maximum new + backup commands
    if (in_rapidfire)
    {
        // Warp right before the next shot
        float shot_time = getFireDelay();
        // This is to prevent Minigun/Pistol from only shooting once
        if (TICKS_TO_TIME(22) / shot_time >= 2)
            shot_time = TICKS_TO_TIME(23);
        return std::min(22, TIME_TO_TICKS(shot_time) - 2);
        // return 22;
    }
    // No limit set
    if (!*maxusrcmdprocessticks)
        max_extra_ticks = INT_MAX;
    float warp_amount_preprocessed = std::max(*speed, 0.05f);

    // How many ticks to warp, add excess too
    int warp_amount_processed = std::floor(warp_amount_preprocessed) + std::floor(excess_ticks);

    // Remove the used amount from excess
    if (finalTick)
        excess_ticks -= std::floor(excess_ticks);

    // Store excess
    if (finalTick)
        excess_ticks += warp_amount_preprocessed - std::floor(warp_amount_preprocessed);

    // Return smallest of the two
    return std::min(warp_amount_processed, max_extra_ticks);
}

DetourHook cl_move_detour;
typedef void (*CL_Move_t)(float accumulated_extra_samples, bool bFinalTick);

// Warping part, uses CL_Move
void Warp(float accumulated_extra_samples, bool finalTick)
{
    auto ch = g_IEngine->GetNetChannelInfo();
    if (!ch)
        return;
    if (!should_warp)
    {
        if (finalTick)
            should_warp = true;
        return;
    }

    PROF_SECTION(warp_profiler);

    int warp_ticks = warp_amount;
    if (warp_amount_override)
        warp_ticks = warp_amount_override;

    CL_Move_t original = (CL_Move_t) cl_move_detour.GetOriginalFunc();

    // Call CL_Move once for every warp tick
    int warp_amnt = GetWarpAmount(finalTick);
    if (warp_amnt)
    {
        int calls = std::min(warp_ticks, warp_amnt);

        // Starts at 1 for the previous packet we already stored
        int packets_sent = 1;
        for (int i = 0; i < calls; i++)
        {
            if (!i)
                first_warp_tick = true;
            else
                first_warp_tick = false;
            // Choke unless we sent too many already
            choke_packet = true;

            // We are sending the last one that fits in this clc_move, stop choking and send them all off
            if (packets_sent == 21 || i == calls - 1)
            {
                choke_packet = false;
                packets_sent = -1;
            }
            else
                hooked_methods::UpdatePred();

            if (in_rapidfire)
                hacks::shared::aimbot::last_target_ignore_timer = tickcount + 12;
            original(accumulated_extra_samples, finalTick);
            // Only decrease ticks for the final CL_Move tick
            if (finalTick)
            {
                warp_amount--;
                warp_ticks--;
            }
            packets_sent++;
        }
        ticks_to_add = 0;
    }
    cl_move_detour.RestorePatch();

    if (warp_amount_override)
        warp_amount_override = warp_ticks;

    if (warp_ticks <= 0)
    {
        was_hurt   = false;
        warp_ticks = 0;
        if (warp_amount_override)
            warp_amount_override = 0;
    }
}

int GetMaxWarpTicks()
{
    int ticks = *maxusrcmdprocessticks;
    // No limit set
    if (!ticks)
        ticks = INT_MAX;
    return ticks;
}

// Approximate demoknight shield speed at a given tick
float approximateSpeedAtTick(int ticks_since_start, float initial_speed, float max_speed)
{
    // Formula only holds up until like 20 ticks
    float speed = ticks_since_start >= 20 ? max_speed : (ticks_since_start * (113.8f - 2.8f * ticks_since_start) + 1.0f);
    return std::min(max_speed * g_GlobalVars->interval_per_tick, initial_speed * g_GlobalVars->interval_per_tick + speed * g_GlobalVars->interval_per_tick);
}

// Approximate the amount of ticks needed for a given distance as demoknight
int approximateTicksForDist(float distance, float initial_speed, int max_ticks)
{
    bool is_skullcutter = false;
    bool has_booties    = false;
    if (CE_GOOD(LOCAL_E) && LOCAL_E->m_bAlivePlayer() && CE_GOOD(LOCAL_W))
    {
        // We have a skullcutter, mark as such
        if (CE_INT(LOCAL_W, netvar.iItemDefinitionIndex) == 172)
            is_skullcutter = true;
        // We have the Booties, mark as such
        if (HasWeapon(LOCAL_E, 405) || HasWeapon(LOCAL_E, 608))
            has_booties = true;
    }
    float travelled_dist = 0.0f;
    for (int i = 0; i <= max_ticks; i++)
    {
        // Compensate for the skullcutter (booties don't speed up, besides with skullcutter)
        travelled_dist += approximateSpeedAtTick(i, initial_speed, is_skullcutter ? (has_booties ? 701.0f : 637.0f) : 750.0f);

        // We hit the needed range
        if (travelled_dist >= distance)
            return i;
    }
    // Not within Max range
    return -1;
}

static bool move_last_tick     = true;
static bool warp_last_tick     = false;
static bool was_hurt_last_tick = false;
static int ground_ticks        = 0;
// Left and right by default
static std::vector<float> yaw_selections{ 90.0f, -90.0f };

enum charge_state
{
    ATTACK = 0,
    CHARGE,
    WARP,
    DONE
};

enum peek_state
{
    IDLE = 0,
    MOVE_TOWARDS,
    MOVE_BACK,
    STOP
};

charge_state current_state    = ATTACK;
peek_state current_peek_state = IDLE;

// Used to determine when we should start moving back with peek
static int charge_at_start = 0;

// Used to determine when demoknight warp should be running
static bool was_overridden = false;

// How long since we are in one of the revved states?
static int ticks_in_revved     = 0;
static bool replaced_last_tick = false;

// Reset all the revv data
void resetRevvstate()
{
    ticks_in_revved    = 0;
    replaced_last_tick = false;
}

void handleMinigun()
{
    // Minigun rapidfire will not work if you hold m1, so we Revv it up with m2 instead.
    if (rapidfire && warp_amount && CE_GOOD(LOCAL_W) && LOCAL_W->m_iClassID() == CL_CLASS(CTFMinigun))
    {
        // Revving
        if (CE_INT(LOCAL_W, netvar.iWeaponState) == 1)
        {
            // Replace m1 with m2 to make rapidfire work
            if (current_user_cmd->buttons & IN_ATTACK)
            {
                current_user_cmd->buttons |= IN_ATTACK2;
                current_user_cmd->buttons &= ~IN_ATTACK;
                replaced_last_tick = true;
            }
            else
                replaced_last_tick = false;
        }
        // Revved
        else if (CE_INT(LOCAL_W, netvar.iWeaponState) == 3)
        {
            // Once we are revved (via m1) we need to wait 23 ticks before we use rapidfire, else it will fail. Valve jank.
            if (replaced_last_tick && current_user_cmd->buttons & IN_ATTACK)
            {
                if (ticks_in_revved < 23)
                {
                    current_user_cmd->buttons |= IN_ATTACK2;
                    current_user_cmd->buttons &= ~IN_ATTACK;
                    ticks_in_revved++;
                }
                // We waited the 23 ticks, skip over this code now
                else
                    resetRevvstate();
            }
            // No m1 revv/not attacking
            else
                resetRevvstate();
        }
        // Other state was entered, e.g. we unrevved. Reset aswell
        else
            resetRevvstate();
    }
}

// This is called first, it subsequently calls all the CreateMove functions.
void CL_Move_hook(float accumulated_extra_samples, bool bFinalTick)
{
    CL_Move_t original = (CL_Move_t) cl_move_detour.GetOriginalFunc();
    original(accumulated_extra_samples, bFinalTick);
    cl_move_detour.RestorePatch();

    // Should we warp?
    if (shouldWarp(true))
    {
        in_warp = true;
        if (shouldRapidfire())
        {
            in_rapidfire = true;
            if (canInstaZoom())
                in_rapidfire_zoom = true;
        }

        Warp(accumulated_extra_samples, bFinalTick);
        if (warp_amount < GetMaxWarpTicks())
            charged = false;
        in_warp           = false;
        in_rapidfire      = false;
        in_rapidfire_zoom = false;
        was_in_warp       = true;
    }
}

// Run before we call the original, we need to adjust the tickcount on the command
// and the global variable so our time based functions are synced properly.
void CreateMoveEarly()
{
    // Update key state
    key_valid = UpdateRFKey();
    if (hacks::tf2::warp::in_rapidfire && current_user_cmd)
    {
        if (current_user_cmd)
        {
            g_GlobalVars->tickcount++;
            current_user_cmd->tick_count++;
        }
    }
}

static float original_curtime = 0.0f;

// Run before prediction so we can do Faststop logic
void CreateMovePrePredict()
{
    // Attempt to stop fast in place to make movement smoother
    if (in_rapidfire && no_movement)
        FastStop();
    if (in_rapidfire_zoom)
    {
        float adjusted_curtime = g_GlobalVars->curtime;
        // Original curtime we need to use while in here
        if (first_warp_tick)
            original_curtime = g_GlobalVars->curtime;

        // Update the data
        g_pLocalPlayer->bZoomed     = true;
        g_pLocalPlayer->flZoomBegin = adjusted_curtime;
        // Do not exceed headshot time, we can only hs next tick
        if (adjusted_curtime - original_curtime > 0.2f)
        {
            g_pLocalPlayer->flZoomBegin = original_curtime - 0.2f - TICKS_TO_TIME(1);
        }
    }
}

// This calls the warp logic and applies some rapidfire specific logic afterwards
// if it applies.
void CreateMove()
{
    warpLogic();
    // Either in rapidfire, or the tick just after. Either way we need to force bSendPackets in some way.
    bool should_rapidfire = shouldRapidfire();
    if (in_rapidfire || should_rapidfire || was_in_warp)
    {
        // If choke packet is set or we are about to rapidfire, choke the packet (Latter is to ensure it is in the same batch as our rapidfire ones)
        if (choke_packet || should_rapidfire)
            *bSendPackets = false;

        // We either just stopped warping, or are on the last tick of rapidfire and want to forcefully release all the usercmds in one packet.
        else
            *bSendPackets = true;

        was_in_warp = false;
    }
}

// Does all the logic related to charging and mode/settings specific actions like peek warp
// and demoknight mode.
void warpLogic()
{
    if (!enabled)
        return;
    if (CE_BAD(LOCAL_E) || !LOCAL_E->m_bAlivePlayer())
        return;
    if (CE_BAD(LOCAL_W))
        return;

    // Handle minigun in rapidfire
    handleMinigun();
    // Handle sniper in rapidfire
    handleSniper();

    // Charge logic
    if (!shouldWarp(false))
    {
        warp_last_tick     = false;
        current_state      = ATTACK;
        current_peek_state = IDLE;

        // Charge warp
        if (charge_key && charge_key.isKeyDown())
        {
            should_charge = true;
            return;
        }

        Vector velocity{};
        velocity::EstimateAbsVelocity(RAW_ENT(LOCAL_E), velocity);

        if (!charge_in_jump)
        {
            if (CE_INT(LOCAL_E, netvar.iFlags) & FL_ONGROUND)
                ground_ticks++;
            else
                ground_ticks = 0;
        }

        bool button_block = (current_user_cmd->buttons & (IN_ATTACK | IN_ATTACK2));
        // Charge on minigun even with m2 held
        if (LOCAL_E->m_bAlivePlayer() && CE_GOOD(LOCAL_W) && LOCAL_W->m_iClassID() == CL_CLASS(CTFMinigun))
            button_block = current_user_cmd->buttons & IN_ATTACK;

        // Bunch of checks, if they all pass we are standing still
        if ((ground_ticks > 1 || charge_in_jump) && (charge_no_input || velocity.IsZero()) && !HasCondition<TFCond_Charging>(LOCAL_E) && !current_user_cmd->forwardmove && !current_user_cmd->sidemove && !current_user_cmd->upmove && !(current_user_cmd->buttons & IN_JUMP) && !button_block)
        {
            if (!move_last_tick)
                should_charge = true;
            move_last_tick = false;

            return;
        }
        else if (charge_passively && (charge_in_jump || ground_ticks > 1))
        {
            bool button_block = (current_user_cmd->buttons & (IN_ATTACK | IN_ATTACK2));
            // Charge on minigun even with m2 held
            if (LOCAL_W->m_iClassID() == CL_CLASS(CTFMinigun))
                button_block = current_user_cmd->buttons & IN_ATTACK;

            if (!button_block)
            {
                // Use every xth tick for charging
                if (*warp_movement_ratio > 0 && !(tickcount % *warp_movement_ratio))
                    should_charge = true;
                move_last_tick = true;
            }
        }
    }
    // Ignore the rest if rapidfire is/should be running
    else if (shouldRapidfire() || in_rapidfire)
        return;
    // Warp when hurt
    else if (was_hurt)
    {
        // Store direction
        static float yaw = 0.0f;
        // Select yaw
        if (!was_hurt_last_tick)
        {
            yaw = 0.0f;
            if (yaw_selections.empty())
                return;
            // Select randomly
            yaw = yaw_selections[UniformRandomInt(0, yaw_selections.size() - 1)];
        }
        // The yaw we want to achieve
        float actual_yaw = DEG2RAD(yaw);
        if (g_pLocalPlayer->bUseSilentAngles)
            actual_yaw = DEG2RAD(yaw);

        // Set forward/sidemove
        current_user_cmd->forwardmove = cos(actual_yaw) * 450.0f;
        current_user_cmd->sidemove    = -sin(actual_yaw) * 450.0f;
    }
    // Demoknight Warp
    else if (warp_demoknight)
    {
        switch (current_state)
        {
        case ATTACK:
        {
            // Don't charge if we do not have atleast half of our warp bar
            if (warp_amount < floor(GetMaxWarpTicks() / 2.0f))
                break;
            // Get charge meter (0 - 100 range)
            float charge_meter = re::CTFPlayerShared::GetChargeMeter(re::CTFPlayerShared::GetPlayerShared(RAW_ENT(LOCAL_E)));

            // If our charge meter is full
            if (charge_meter == 100.0f)
            {
                // Find an entity meeting the Criteria and closest to crosshair
                std::pair<CachedEntity *, float> result{ nullptr, FLT_MAX };

                for (int i = 1; i <= g_IEngine->GetMaxClients(); i++)
                {
                    CachedEntity *ent = ENTITY(i);
                    if (CE_BAD(ent) || !ent->m_bAlivePlayer() || !ent->m_bEnemy() || !player_tools::shouldTarget(ent))
                        continue;
                    // No hitboxes
                    if (!ent->hitboxes.GetHitbox(2))
                        continue;
                    float FOVScore = GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, ent->hitboxes.GetHitbox(spine_1)->center);
                    if (FOVScore < result.second)
                    {
                        result.second = FOVScore;
                        result.first  = ent;
                    }
                }
                // We found a good entity within range, Set needed warp amount
                if (result.first)
                {
                    float distance = LOCAL_E->m_vecOrigin().DistTo(result.first->m_vecOrigin());

                    // This approximates the ticks needed for the distance
                    Vector vel;
                    velocity::EstimateAbsVelocity(RAW_ENT(LOCAL_E), vel);
                    // +11 to account for the melee delay, the -40.0f is so we don't just aim for the center, which would
                    // Make our range artifically shorter
                    int charge_ticks = approximateTicksForDist(distance - 40.0f, vel.Length(), warp_amount + 11);

                    // Not in range, try again with melee range taken into compensation
                    if (charge_ticks <= 0)
                        charge_ticks = approximateTicksForDist(distance - 128.0f, vel.Length(), warp_amount + 11);
                    // Out of range
                    if (charge_ticks <= 0)
                    {
                        charge_ticks = warp_amount;
                        should_melee = false;
                    }
                    else
                    {
                        charge_ticks = std::clamp(charge_ticks, 0, warp_amount);
                        // For every started batch We can subtract 1 because we'll get an extra CreateMove call.
                        charge_ticks -= std::ceil(charge_ticks / *speed);
                        should_melee = true;
                    }
                    // Use these ticks
                    warp_amount_override = charge_ticks;

                    was_overridden = true;
                }
                else
                {
                    should_melee   = false;
                    was_overridden = false;
                }

                // Force a crit
                criticals::force_crit_this_tick = true;
                if (should_melee)
                    current_user_cmd->buttons |= IN_ATTACK;
                current_state = CHARGE;
            }
            // Just warp normally if meter isn't full
            else
            {
                was_overridden = false;
                current_state  = WARP;
            }

            should_warp = false;

            break;
        }
        case CHARGE:
        {
            current_user_cmd->buttons |= IN_ATTACK2;
            current_state = WARP;
            should_warp   = false;
            break;
        }
        case WARP:
        {
            should_warp = true;
            if ((was_overridden && !warp_amount_override) || !warp_amount)
            {
                should_warp   = false;
                current_state = DONE;
            }
            break;
        }
        case DONE:
        {
            // Since we are done Warping we should stop trying to use any more
            should_warp = false;
            break;
        }
        default:
            break;
        }
    }
    // Warp peaking
    else if (warp_peek)
    {
        switch (current_peek_state)
        {
        case IDLE:
        {
            // Not doing anything, update warp amount
            charge_at_start = warp_amount;

            Vector vel;
            velocity::EstimateAbsVelocity(RAW_ENT(LOCAL_E), vel);

            // if we move more than 1.0 HU/s and buttons are pressed, and we are grounded, go to move towards statement...
            if (CE_INT(LOCAL_E, netvar.iFlags) & FL_ONGROUND && !vel.IsZero(1.0f) && current_user_cmd->buttons & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT))
                current_peek_state = MOVE_TOWARDS;
            // ...else don't warp
            else
            {
                should_warp = false;
                break;
            }

            [[fallthrough]];
        }
        case MOVE_TOWARDS:
        {
            // Just wait until we used about a third of our warp, the rest has to be used for moving back
            if (warp_amount <= charge_at_start * (2.0f / 3.0f))
                current_peek_state = MOVE_BACK;
            break;
        }
        case MOVE_BACK:
        {
            // Inverse direction if we still have warp left
            if (warp_amount)
            {
                current_user_cmd->forwardmove *= -1.0f;
                current_user_cmd->sidemove *= -1.0f;
                break;
            }
            // ... Else we stop in our tracks
            else
                current_peek_state = STOP;

            [[fallthrough]];
        }
        case STOP:
        {
            // Stop dead in our tracks while key is still held
            current_user_cmd->forwardmove = 0.0f;
            current_user_cmd->sidemove    = 0.0f;
            break;
        }
        default:
            break;
        }
    }
    was_hurt_last_tick = was_hurt;
}

// The second to last thing that gets called, its only job is to write the commands locally and then queue for sending.
// We simply make the "backup" command buffer accessible which allows us to send more at once.
//
// Only called if *bSendPackets is true.
void CL_SendMove_hook()
{
    byte data[4000];

    // the +4 one is choked commands
    int nextcommandnr = NET_INT(g_IBaseClientState, offsets::lastoutgoingcommand()) + NET_INT(g_IBaseClientState, offsets::lastoutgoingcommand() + 4) + 1;

    // send the client update packet

    CLC_Move moveMsg;

    moveMsg.m_DataOut.StartWriting(data, sizeof(data));

    // Determine number of backup commands to send along
    int cl_cmdbackup = 2;

    // How many real new commands have queued up
    moveMsg.m_nNewCommands = 1 + NET_INT(g_IBaseClientState, offsets::lastoutgoingcommand() + 4);
    moveMsg.m_nNewCommands = std::clamp(moveMsg.m_nNewCommands, 0, 15);

    // Excessive commands (Used for longer fakelag, credits to https://www.unknowncheats.me/forum/source-engine/370916-23-tick-guwop-fakelag-break-lag-compensation-running.html)
    int extra_commands        = NET_INT(g_IBaseClientState, offsets::lastoutgoingcommand() + 4) + 1 - moveMsg.m_nNewCommands;
    cl_cmdbackup              = std::max(2, extra_commands);
    moveMsg.m_nBackupCommands = std::clamp(cl_cmdbackup, 0, 7);

    int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

    int from = -1; // first command is deltaed against zeros

    bool bOK = true;

    for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++)
    {
        bool isnewcmd = to >= (nextcommandnr - moveMsg.m_nNewCommands + 1);

        // Call the write to buffer
        typedef bool (*WriteUsercmdDeltaToBuffer_t)(IBaseClientDLL *, bf_write *, int, int, bool);

        // first valid command number is 1
        bOK  = bOK && vfunc<WriteUsercmdDeltaToBuffer_t>(g_IBaseClient, offsets::PlatformOffset(23, offsets::undefined, offsets::undefined), 0)(g_IBaseClient, &moveMsg.m_DataOut, from, to, isnewcmd);
        from = to;
    }

    if (bOK)
    {
        // Make fakelag work as we want it to
        if (extra_commands > 0)
            ((INetChannel *) g_IEngine->GetNetChannelInfo())->m_nChokedPackets -= extra_commands;

        // only write message if all usercmds were written correctly, otherwise parsing would fail
        ((INetChannel *) g_IEngine->GetNetChannelInfo())->SendNetMsg(moveMsg);
    }
}

// Called after CL_SendMove to transmit the clc_move message. We choke it if we want to charge warp.
void SendNetMessage(INetMessage &msg)
{
    if (!enabled)
        return;
    // Credits to MrSteyk for this btw
    if (msg.GetGroup() == INetChannelInfo::MOVE)
    {
        // Charge
        if (should_charge && !charged)
        {
            int ticks    = GetMaxWarpTicks();
            auto movemsg = (CLC_Move *) &msg;

            // Just null it :shrug:
            movemsg->m_nBackupCommands = 0;
            movemsg->m_nNewCommands    = 0;
            movemsg->m_DataOut.Reset();
            movemsg->m_DataOut.m_nDataBits  = 0;
            movemsg->m_DataOut.m_nDataBytes = 0;
            movemsg->m_DataOut.m_iCurBit    = 0;

            warp_amount++;
            if (warp_amount >= ticks)
            {
                warp_amount = ticks;
                charged     = true;
            }
        }
    }
    should_charge = false;
}

#if ENABLE_VISUALS
void Draw()
{
    if (!enabled)
        return;
    if (!draw && !draw_bar)
        return;
    if (!g_IEngine->IsInGame())
        return;
    if (CE_BAD(LOCAL_E))
        return;
    if (!LOCAL_E->m_bAlivePlayer())
        return;

    if (draw)
    {
        rgba_t color = colors::orange;
        if (warp_amount == 0)
            color = colors::FromRGBA8(128.0f, 128.0f, 128.0f, 255.0f);
        else if (GetMaxWarpTicks() == warp_amount)
            color = colors::green;
        AddWarpString("Shiftable ticks: " + std::to_string(warp_amount), color);
    }

    if (draw_bar)
    {
        float charge_percent = (float) warp_amount / (float) GetMaxWarpTicks();
        // Draw background
        static rgba_t background_color = colors::FromRGBA8(96, 96, 96, 150);
        float bar_bg_x_size            = *size * 2.0f;
        float bar_bg_y_size            = *size / 5.0f;
        draw::Rectangle(*bar_x - 5.0f, *bar_y - 5.0f, bar_bg_x_size + 10.0f, bar_bg_y_size + 10.0f, background_color);
        // Draw bar
        rgba_t color_bar = colors::orange;
        if (GetMaxWarpTicks() == warp_amount)
            color_bar = colors::green;
        color_bar.a = 100 / 255.0f;
        draw::Rectangle(*bar_x, *bar_y, *size * 2.0f * charge_percent, *size / 5.0f, color_bar);
    }

    DrawWarpStrings();
}
#endif

void LevelShutdown()
{
    charged     = false;
    warp_amount = 0;
}

class WarpHurtListener : public IGameEventListener2
{
public:
    virtual void FireGameEvent(IGameEvent *event)
    {
        // Not enabled
        if (!isHackActive() || !enabled || !warp_on_damage)
            return;
        // We have no warp
        if (!warp_amount)
            return;
        // Store userids
        int victim       = event->GetInt("userid");
        int attacker     = event->GetInt("attacker");
        int attacker_idx = GetPlayerForUserID(attacker);
        int victim_idx   = GetPlayerForUserID(victim);
        player_info_s kinfo{};
        player_info_s vinfo{};

        // Check if both are valid (Attacker & victim)
        if (IDX_BAD(attacker_idx) || IDX_BAD(victim_idx) || !GetPlayerInfo(attacker_idx, &vinfo) || !GetPlayerInfo(victim_idx, &kinfo))
            return;
        // Check if victim is local player
        if (victim_idx != g_pLocalPlayer->entity_idx)
            return;

        // Check if the entities are alive and valid
        CachedEntity *att = ENTITY(attacker_idx);

        // Don't run if we (the victim) are invalid
        if (CE_BAD(LOCAL_E) || !LOCAL_E->m_bAlivePlayer())
            return;
        // Ignore projectile damage for now
        if (CE_VALID(att) && att->m_bAlivePlayer() && GetWeaponMode(att) == weapon_projectile)
            return;

        // We got hurt
        was_hurt = true;
    }
};

static WarpHurtListener listener;

void rvarCallback(settings::VariableBase<bool> &, bool)
{
    yaw_selections.clear();
    if (warp_forward)
        yaw_selections.push_back(0.0f);
    if (warp_backwards)
        yaw_selections.push_back(-180.0f);
    if (warp_left)
        yaw_selections.push_back(-90.0f);
    if (warp_right)
        yaw_selections.push_back(90.0f);
}

static InitRoutine init(
    []()
    {
        static auto cl_move_addr = gSignatures.GetEngineSignature("55 89 E5 57 56 53 81 EC 9C 00 00 00 83 3D ? ? ? ? 01");
        cl_move_detour.Init(cl_move_addr, (void *) CL_Move_hook);

        EC::Register(EC::LevelShutdown, LevelShutdown, "warp_levelshutdown");
        EC::Register(EC::CreateMove, CreateMove, "warp_createmove", EC::very_late);
        EC::Register(EC::CreateMoveWarp, CreateMove, "warp_createmovew", EC::very_late);
        EC::Register(EC::CreateMove_NoEnginePred, CreateMovePrePredict, "warp_prepredict");
        EC::Register(EC::CreateMoveEarly, CreateMoveEarly, "warp_createmove_early", EC::very_early);
        g_IEventManager2->AddListener(&listener, "player_hurt", false);
        EC::Register(
            EC::Shutdown,
            []()
            {
                g_IEventManager2->RemoveListener(&listener);
                cl_move_detour.Shutdown();
            },
            "warp_shutdown");
        warp_forward.installChangeCallback(rvarCallback);
        warp_backwards.installChangeCallback(rvarCallback);
        warp_left.installChangeCallback(rvarCallback);
        warp_right.installChangeCallback(rvarCallback);

#if ENABLE_VISUALS
        EC::Register(EC::Draw, Draw, "warp_draw");
#endif
    });
} // namespace hacks::tf2::warp
