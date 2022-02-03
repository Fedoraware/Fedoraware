#include "ChatInfo.h"

#include "../Vars.h"
#include "../Misc/Misc.h"
#include "../Crits/Crits.h"
#include "../../Features/Visuals/Visuals.h" // u dont see this

int attackStringW;
int attackStringH;

#define GET_PLAYER_USERID(userid) g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(userid))
#define GET_INDEX_USERID(userid) g_Interfaces.Engine->GetPlayerForUserID(userid)

void CChatInfo::Event(CGameEvent* pEvent, const FNV1A_t uNameHash) {
	if (!g_Interfaces.Engine->IsConnected() || !g_Interfaces.Engine->IsInGame())
		return;

	//static std::string clr({ '\x7', 'F', 'F', 'C', '1', '4', 'B' }); // this colour looks like poo
	static std::string clr({ '\x7', '0', 'D', '9', '2', 'F', 'F' }); // this colour looks P!!!

	if (const auto pLocal = g_EntityCache.m_pLocal) {
		if (Vars::Visuals::ChatInfo.m_Var) {

			// literally completely pasted from deathpole
			if (Vars::Misc::VoteRevealer.m_Var && uNameHash == FNV1A::HashConst("vote_started")) { // IT TOOK ME SO LONG TO REALISE I HADN'T ADDED THE LISTENER
				const auto initiator = pEvent->GetInt("initiator"); 
				const auto target = pEvent->GetString("param1"); // im almost certain this is the param for the targets name 

				PlayerInfo_t pii;
				g_Interfaces.Engine->GetPlayerInfo(initiator, &pii);

				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD] Vote Initiator: \x3%s", clr,  pii.name).c_str());
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD] Vote Target: \x3%s", clr,  target).c_str());
			}

			if (Vars::Misc::VoteRevealer.m_Var && uNameHash == FNV1A::HashConst("vote_cast")) {
				const auto pEntity = g_Interfaces.EntityList->GetClientEntity(pEvent->GetInt("entityid"));
				if (pEntity && pEntity->IsPlayer()) {
					const bool bVotedYes = pEvent->GetInt("vote_option") == 0;
					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(pEntity->GetIndex(), &pi);
					std::string voteString;
					voteString = "[FeD] " + std::string((pEntity->GetTeamNum() != pLocal->GetTeamNum()) ? "[enemy] " : "") + std::string(pi.name) + " voted " + std::string(bVotedYes ? "Yes" : "No");
					g_notify.Add(voteString);
					//g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD] \x3%s voted %s", clr, pi.name, bVotedYes ? "Yes" : "No").c_str());
					g_Interfaces.CVars->ConsoleColorPrintf({ 133, 255, 66, 255 }, _("%s\n"), voteString.c_str());
					if (Vars::Misc::VotesInChat.m_Var) {
						g_Interfaces.Engine->ClientCmd_Unrestricted(tfm::format("say_party \"%s voted %s\"", pi.name, bVotedYes ? "Yes" : "No").c_str());
					}
				}
			}

			// i have moved this to g_notify because it WILL look better than spamming ur chat for every idiot hvher tryna change spawns on 2fort
			if (uNameHash == FNV1A::HashConst("player_changeclass")) {
				if (const auto& pEntity = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt("userid")))) {
					if (pEntity == pLocal) { return; }
					int nIndex = pEntity->GetIndex();

					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi);

					std::string classString = "[FeD] " + std::string(pi.name) + " is now a " + std::string(Utils::GetClassByIndex(pEvent->GetInt("class"))).c_str();
					std::wstring wclassString = std::wstring(classString.begin(), classString.end());
					g_notify.Add(classString);

					//g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nIndex, tfm::format("%s[dp] \x3%s\x1 is now a \x3%s\x1!", clr, pi.name, Utils::GetClassByIndex(pEvent->GetInt("class"))).c_str());
				}
			}

			if (uNameHash == FNV1A::HashConst("player_connect")) {
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), tfm::format("\x3%s\x1 connected. (%s)", pEvent->GetString("name"), pEvent->GetString("address")).c_str());
			}
		}

		if (Vars::Visuals::damageLogger.m_Var && uNameHash == FNV1A::HashConst("player_hurt")) {
			if (const auto pEntity = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt("userid")))) {
				if (pEntity == pLocal) { return; } // u could literally hurt urself what a meme
				const auto nAttacker = pEvent->GetInt("attacker");
				const auto nHealth = pEvent->GetInt("health");
				const auto nDamage = pEvent->GetInt("damageamount");
				const auto bCrit = pEvent->GetBool("crit");
				const int  nIndex = pEntity->GetIndex();

				PlayerInfo_t pi;

				{
					g_Interfaces.Engine->GetPlayerInfo(g_Interfaces.Engine->GetLocalPlayer(), &pi);
					if (nAttacker != pi.userID)
						return;
				}

				g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi);

				g_Crits.RoundDamage += nDamage;

				if (bCrit) {
					g_Crits.CritDamage += (float)nDamage;
				}

				const auto maxHealth = pEntity->GetMaxHealth();
				std::string attackString = "You hit " + std::string(pi.name) + " for " + std::to_string(nDamage) + (bCrit ? " (crit) " : " ") + "(" + std::to_string(nHealth) + "/" + std::to_string(maxHealth) + ")";

				std::wstring wattackString = std::wstring(attackString.begin(), attackString.end());
				const wchar_t* wcattackString = wattackString.c_str();
				g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_ESP_COND].dwFont, wcattackString, attackStringW, attackStringH);

				if (Vars::Visuals::damageLogger.m_Var == 1 && Vars::Visuals::ChatInfo.m_Var) // who tf uses this
					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD]\x3 %s", clr, attackString.c_str()).c_str());

				if (Vars::Visuals::damageLogger.m_Var == 2)
					g_notify.Add(attackString);
			}
		}

		if (Vars::Aimbot::Global::showHitboxes.m_Var && uNameHash == FNV1A::HashConst("player_hurt")) {
			if (Vars::Aimbot::Global::clearPreviousHitbox.m_Var) { g_Interfaces.DebugOverlay->ClearAllOverlays(); }
			auto time = Vars::Aimbot::Global::hitboxTime.m_Var;
			auto colour = Colors::Hitbox; // alpha is how "filled" the hitbox render is, looks bad at anything non-zero (rijin moment)
			auto pEntity = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt("userid")));
			const auto nAttacker = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt("attacker")));
			if (pEntity == pLocal) { return; }; if (pLocal != nAttacker) { return; }
			g_Visuals.DrawHitboxMatrix(pEntity, colour, time);
		}
		

		if (uNameHash == FNV1A::HashConst("achievement_earned")) {
			const int player = pEvent->GetInt("player", 0xDEAD);
			const int achievement = pEvent->GetInt("achievement", 0xDEAD);

			// 0xCA7 is an identify and mark request.
			// 0xCA8 is a mark request.

			PlayerInfo_t info;
			if (g_Interfaces.Engine->GetPlayerInfo(player, &info) && (achievement == 0xCA7 || achievement == 0xCA8) && pLocal->GetIndex() != player) {
				if (m_known_bots.find(info.friendsID) == m_known_bots.end()) {
					g_notify.Add(tfm::format("%s is a bot!", info.name));
					if (Vars::Visuals::ChatInfo.m_Var)
						g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(player, tfm::format("%s[FeD] \x3 %s\x1 is a bot!", clr, info.name).c_str());

					{ // marked by other bots. r.i.p cl_drawline :(
						// this will be detected by fedoraware and lmaobox easily.
						// use 0xCA7 if you want to make more bots do the thing,
						// most only care about being marked.
						if (Vars::Misc::BeCat.m_Var) {
							KeyValues* kv = new KeyValues("AchievementEarned");
							kv->SetInt("achievementID", 0xCA8);
							g_Interfaces.Engine->ServerCmdKeyValues(kv);
						}
					}

					m_known_bots[info.friendsID] = true;
				}
			}
		}
	}

	/*if (uNameHash == FNV1A::HashConst("player_death")) {
		short userid = pEvent->GetInt("userid");									//user ID who died 
		long 	victim_entindex = pEvent->GetInt("victim_entindex");
		long 	inflictor_entindex = pEvent->GetInt("inflictor_entindex");			//ent index of inflictor(a sentry, for example)
		short 	attacker = pEvent->GetInt("attacker");								//user ID who killed
		const char* weapon = pEvent->GetString("weapon");							//weapon name killer used
		short 	weaponid = pEvent->GetInt("weaponid"); 								//ID of weapon killed used
		long 	damagebits = pEvent->GetInt("damagebits");							//bits of type of damage
		short 	customkill = pEvent->GetInt("customkill"); 							//type of custom kill
		short 	assister = pEvent->GetInt("assister"); 								//user ID of assister
		const char* weapon_logclassname = pEvent->GetString("weapon_logclassname");	//weapon name that should be printed on the log
		short 	stun_flags = pEvent->GetInt("assister");							//victim's stun flags at the moment of death
		short 	death_flags = pEvent->GetInt("death_flags");						//death flags.
		bool 	silent_kill = pEvent->GetBool("silent_kill");
		short 	playerpenetratecount = pEvent->GetInt("playerpenetratecount");
		const char* assister_fallback = pEvent->GetString("assister_fallback");		//contains a string to use if "assister" is - 1
		short 	kill_streak_total = pEvent->GetInt("kill_streak_total");			//Kill streak count(level)
		short 	kill_streak_wep = pEvent->GetInt("kill_streak_wep");				//Kill streak for killing weapon
		short 	kill_streak_assist = pEvent->GetInt("kill_streak_assist");			//Kill streak for assister count
		short 	kill_streak_victim = pEvent->GetInt("kill_streak_victim");			//Victims kill streak
		short 	ducks_streaked = pEvent->GetInt("ducks_streaked");					//Duck streak increment from this kill
		short 	duck_streak_total = pEvent->GetInt("duck_streak_total");			//Duck streak count for attacker
		short 	duck_streak_assist = pEvent->GetInt("duck_streak_assist");			//Duck streak count for assister
		short 	duck_streak_victim = pEvent->GetInt("duck_streak_victim");			//(former) duck streak count for victim
		bool 	rocket_jump = pEvent->GetBool("rocket_jump");						//was the victim rocket jumping
		short 	weapon_def_index = pEvent->GetInt("weapon_def_index");				//item def index of weapon killer used
		short 	crit_type = pEvent->GetInt("crit_type");							//Crit type of kill. (0: None, 1 : Mini, 2 : Full)
		//g_Interfaces.CVars->ConsolePrintf("weapon: %s", weapon);
		//g_Interfaces.CVars->ConsolePrintf("weapon_logclassname: %s", weapon_logclassname);
		//g_Interfaces.CVars->ConsolePrintf("userid: %d\nvictim_entindex: %d\nattacker: %d\nweapon: %s\nweaponid: %d\ndamagebits: %d\n", userid, victim_entindex, inflictor_entindex, attacker, weapon, weaponid, damagebits); 
		//g_Interfaces.CVars->ConsolePrintf("customkill: %d\nassister: %d\nweapon_logclassname: %s\nstun_flags: %d\ndeath_flags: %d\nsilent_kill: %d\nplayerpenetratecount: %d\n", customkill, assister, weapon_logclassname, stun_flags, death_flags, silent_kill, playerpenetratecount);
		//g_Interfaces.CVars->ConsolePrintf("assister_fallback: %s\nkill_streak_total: %d\nkill_streak_wep: %d\nkill_streak_assist: %d\nkill_streak_victim: %d\nducks_streaked: %d\nduck_streak_assist: %d\nduck_streak_victim: %d\n", assister_fallback, kill_streak_total, kill_streak_wep, kill_streak_assist, kill_streak_victim, ducks_streaked, duck_streak_total, duck_streak_assist, duck_streak_victim);
		//g_Interfaces.CVars->ConsolePrintf("rocket_jump: %d\nweapon_def_index: %d\ncrit_type: %d\n", rocket_jump, weapon_def_index, crit_type);
	}*/
}