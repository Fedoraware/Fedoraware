#include "ClientHook.h"

#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../Features/PlayerList/PlayerList.h"

const static std::string clear("?\nServer:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

static std::string clr({ '\x7', '0', 'D', '9', '2', 'F', 'F' });

void __stdcall ClientHook::PreEntity::Hook(char const* szMapName)
{
	Table.Original<fn>(index)(g_Interfaces.Client, szMapName);
}

void __stdcall ClientHook::PostEntity::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_maxdlights 69420"));
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_dynamic 1"));
	g_Visuals.ModulateWorld();
}

void __stdcall ClientHook::ShutDown::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);
	g_EntityCache.Clear();
}

void __stdcall ClientHook::FrameStageNotify::Hook(EClientFrameStage FrameStage)
{
	switch (FrameStage)
	{
	case EClientFrameStage::FRAME_RENDER_START:
	{
		g_GlobalInfo.m_vPunchAngles = Vec3();

		if (Vars::Visuals::RemovePunch.m_Var) {
			if (const auto& pLocal = g_EntityCache.m_pLocal) {
				g_GlobalInfo.m_vPunchAngles = pLocal->GetPunchAngles();	//Store punch angles to be compesnsated for in aim
				pLocal->ClearPunchAngle();								//Clear punch angles for visual no-recoil
			}
		}

		if (Vars::AntiHack::Resolver::Resolver.m_Var)
		{
			for (auto i = 1; i <= g_Interfaces.Engine->GetMaxClients(); i++)
			{
				CBaseEntity* entity = nullptr;
				PlayerInfo_t temp;

				if (!(entity = g_Interfaces.EntityList->GetClientEntity(i)))
					continue;

				if (entity->GetDormant())
					continue;

				if (!g_Interfaces.Engine->GetPlayerInfo(i, &temp))
					continue;

				if (!entity->GetLifeState() == LIFE_ALIVE)
					continue;

				if (entity->IsTaunting())
					continue;

				Vector vX = entity->GetEyeAngles();
				auto* m_angEyeAnglesX = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + g_NetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]"));
				auto* m_angEyeAnglesY = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + g_NetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]"));

				auto findResolve = g_GlobalInfo.resolvePlayers.find(temp.friendsID);
				ResolveMode resolveMode;
				if (findResolve != g_GlobalInfo.resolvePlayers.end()) {
					resolveMode = findResolve->second;
				}

				// Pitch resolver 
				switch (resolveMode.m_Pitch) {
				case 1: {
					*m_angEyeAnglesX = -89; // Up
					break;
				}
				case 2: {
					*m_angEyeAnglesX = 89;  // Down
					break;
				}
				case 3: {
					*m_angEyeAnglesX = 0;  // Zero
					break;
				}
				case 4: {
					// Auto (Will resolve fake up/down)
					if (vX.x >= 90)
					{
						*m_angEyeAnglesX = -89;
					}

					if (vX.x <= -90)
					{
						*m_angEyeAnglesX = 89;
					}
					break;
				}
				default:
					break;
				}

				Vec3 vPos;
				if (const auto& pLocal = g_EntityCache.m_pLocal) {
					vPos = pLocal->GetHitboxPos(HITBOX_HEAD);
				}
				Vec3 vAngleTo = Math::CalcAngle(entity->GetHitboxPos(HITBOX_HEAD), vPos);

				// Yaw resolver
				switch (resolveMode.m_Yaw)
				{
				case 1: {
					*m_angEyeAnglesY = vAngleTo.y;  // Forward
					break;
				}
				case 2: {
					*m_angEyeAnglesY = vAngleTo.y + 180.f; // Backward
					break;
				}
				case 3: {
					*m_angEyeAnglesY = vAngleTo.y - 90.f; // Left
					break;
				}
				case 4: {
					*m_angEyeAnglesY = vAngleTo.y + 90.f;  // Right
					break;
				}
				case 5: {
					*m_angEyeAnglesY += 180; // Invert (this doesn't work properly)
					break;
				}
				default:
					break;
				}
			}
		}
		g_Visuals.ThirdPerson();
		g_Visuals.SkyboxChanger();

		break;
	}

	default: break;
	}

	g_Visuals.BigHeads(Vars::ESP::Players::Headscale.m_Var,
		Vars::ESP::Players::Torsoscale.m_Var,
		Vars::ESP::Players::Handscale.m_Var);

	Table.Original<fn>(index)(g_Interfaces.Client, FrameStage);

	switch (FrameStage)
	{
	case EClientFrameStage::FRAME_NET_UPDATE_START: {
		g_EntityCache.Clear();
		break;
	}


	case EClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_START: {
		g_AttributeChanger.Run();
		break;
	}


	case EClientFrameStage::FRAME_NET_UPDATE_END:
	{
		g_EntityCache.Fill();
		g_PlayerList.GetPlayers();

		g_GlobalInfo.m_bLocalSpectated = false;

		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			for (const auto& Teammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
			{
				if (Teammate->IsAlive() || g_EntityCache.Friends[Teammate->GetIndex()])
					continue;

				CBaseEntity* pObservedPlayer = g_Interfaces.EntityList->GetClientEntityFromHandle(Teammate->GetObserverTarget());

				if (pObservedPlayer == pLocal)
				{
					switch (Teammate->GetObserverMode()) {
					case OBS_MODE_FIRSTPERSON: break;
					case OBS_MODE_THIRDPERSON: break;
					default: continue;
					}

					g_GlobalInfo.m_bLocalSpectated = true;
					break;
				}
			}
		}

		break;
	}

	case EClientFrameStage::FRAME_RENDER_START: {
		if (!g_GlobalInfo.unloadWndProcHook) {
			g_Visuals.UpdateWorldModulation();
			g_Visuals.UpdateSkyModulation();
		}
		break;
	}

	default: break;
	}
}

bool __stdcall ClientHook::DispatchUserMessage::Hook(int type, bf_read& msg_data)
{
	const char* buf_data = reinterpret_cast<const char*>(msg_data.m_pData);

	switch (type) {
	case 4:
	{
		// Received chat message
		int nbl = msg_data.GetNumBytesLeft();
		if (nbl >= 256) {
			break;
		}

		std::string data;
		for (int i = 0; i < nbl; i++) {
			data.push_back(buf_data[i]);
		}

		const char* p = data.c_str() + 2;
		std::string event(p), name((p += event.size() + 1)), message(p + name.size() + 1);
		int ent_idx = data[0];

		if (Vars::Misc::ChatCensor.m_Var) {
			std::vector<std::string> badWords{ "cheat", "hack", "bot", "aim", "esp", "kick", "hax" };
			bool bwFound = false;
			for (std::string word : badWords) {
				if (strstr(message.c_str(), word.c_str())) {
					bwFound = true;
					break;
				}
			}

			if (bwFound) {
				std::string cmd = "say \"" + clear + "\"";
				g_Interfaces.Engine->ServerCmd(cmd.c_str(), '"');
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD] \x3 %s\x1 wrote\x3 %s", clr, name, message).c_str());
			}
		}
		msg_data.Seek(0);
		break;
	}
	}

	return Table.Original<fn>(index)(g_Interfaces.Client, type, msg_data);
}