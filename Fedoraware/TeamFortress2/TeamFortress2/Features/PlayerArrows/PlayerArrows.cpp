#include "PlayerArrows.h"
#include "../Vars.h"

bool CPlayerArrows::ShouldRun(CBaseEntity* pLocal)
{
	if (!Vars::Visuals::OutOfFOVArrows.m_Var || g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	if (!pLocal->IsAlive() || pLocal->IsStunned())
		return false;

	if (pLocal->IsInBumperKart() || pLocal->IsAGhost())
		return false;

	return true;
}

void CPlayerArrows::DrawArrowTo(const Vec3& vecFromPos, const Vec3& vecToPos, Color_t color)
{
	color.a = 150;
	auto GetClockwiseAngle = [&](const Vec3& vecViewAngle, const Vec3& vecAimAngle) -> float
	{
		Vec3 vecAngle = Vec3();
		Math::AngleVectors(vecViewAngle, &vecAngle);

		Vec3 vecAim = Vec3();
		Math::AngleVectors(vecAimAngle, &vecAim);

		return -atan2(vecAngle.x * vecAim.y - vecAngle.y * vecAim.x, vecAngle.x * vecAim.x + vecAngle.y * vecAim.y);
	};

	auto MapFloat = [&](float x, float in_min, float in_max, float out_min, float out_max) -> float {
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	};

	Vec3 vecAngleTo = Math::CalcAngle(vecFromPos, vecToPos);
	Vec3 vecViewAngle = g_Interfaces.Engine->GetViewAngles();

	const float deg = GetClockwiseAngle(vecViewAngle, vecAngleTo);
	const float xrot = cos(deg - PI / 2);
	const float yrot = sin(deg - PI / 2);

	const float x1 = ((g_ScreenSize.w * 0.15) + 5.0f) * xrot;
	const float y1 = ((g_ScreenSize.w * 0.15) + 5.0f) * yrot;
	const float x2 = ((g_ScreenSize.w * 0.15) + 15.0f) * xrot;
	const float y2 = ((g_ScreenSize.w * 0.15) + 15.0f) * yrot;

	//constexpr float arrow_angle = DEG2RAD(60.0f);
	float arrow_angle = DEG2RAD(Vars::Visuals::ArrowAngle.m_Var);
	//constexpr float arrow_lenght = 20.0f;
	float arrow_lenght = Vars::Visuals::ArrowLength.m_Var;

	const Vec3 line{ x2 - x1, y2 - y1, 0.0f };
	const float length = line.Lenght();

	const float fpoint_on_line = arrow_lenght / (atanf(arrow_angle) * length);
	const Vec3 point_on_line = Vec3(x2, y2, 0) + (line * fpoint_on_line * -1.0f);
	const Vec3 normal_vector{ -line.y, line.x, 0.0f };
	const Vec3 normal = Vec3(arrow_lenght, arrow_lenght, 0.0f) / (length * 2);

	const Vec3 rotation = normal * normal_vector;
	const Vec3 left = point_on_line + rotation;
	const Vec3 right = point_on_line - rotation;

	float cx = static_cast<float>(g_ScreenSize.w / 2);
	float cy = static_cast<float>(g_ScreenSize.h / 2);

	//float fMap = std::clamp(MapFloat(vecFromPos.DistTo(vecToPos), 1000.0f, 100.0f, 0.0f, 1.0f), 0.0f, 1.0f);
	float fMap = std::clamp(MapFloat(vecFromPos.DistTo(vecToPos), Vars::Visuals::MaxDist.m_Var, Vars::Visuals::MinDist.m_Var, 0.0f, 1.0f), 0.0f, 1.0f);
	Color_t HeatColor = color;
	HeatColor.a = static_cast<byte>(fMap * 255.0f);

	if (Vars::Visuals::OutOfFOVArrowsOutline.m_Var) {
		g_Draw.Line(cx + x2, cy + y2, cx + left.x, cy + left.y, HeatColor);
		g_Draw.Line(cx + x2, cy + y2, cx + right.x, cy + right.y, HeatColor);
		g_Draw.Line(cx + left.x, cy + left.y, cx + right.x, cy + right.y, HeatColor);
	}
	else {
		Vertex_t t1, t2,t3;
		t1.Init({ cx + left.x, cy + left.y });
		t2.Init({ cx + right.x, cy + right.y });
		t3.Init({ cx + x2, cy + y2 });
		std::array<Vertex_t, 3> verts{ t1,t2,t3 };
		g_Draw.DrawTexturedPolygon(3, verts.data(), HeatColor);
	}
	//g_Draw.
}

void CPlayerArrows::Run()
{

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!ShouldRun(pLocal))
			return;

		Vec3 vLocalPos = pLocal->GetWorldSpaceCenter();

		m_vecPlayers.clear();

		for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!pEnemy || !pEnemy->IsAlive() || pEnemy->IsCloaked() || pEnemy->IsAGhost())
				continue;

			if (Vars::Visuals::SpyWarningIgnoreFriends.m_Var && g_EntityCache.Friends[pEnemy->GetIndex()])
				continue;

			Vec3 vEnemyPos = pEnemy->GetWorldSpaceCenter();

			/*if (!(vLocalPos.DistTo(vEnemyPos) > 400.0f))
				continue;*/

			Vec3 vAngleToEnemy = Math::CalcAngle(vLocalPos, vEnemyPos);
			Vec3 viewangless = g_Interfaces.Engine->GetViewAngles();
			viewangless.x = 0;
			float fFovToEnemy = Math::CalcFov(viewangless, vAngleToEnemy);

			/*
			Vec3 vEntForward = {};
			Math::AngleVectors(pEnemy->GetEyeAngles(), &vEntForward);
			Vec3 vToEnt = pEnemy->GetAbsOrigin() - pLocal->GetAbsOrigin();
			vToEnt.NormalizeInPlace();

			if (vEntForward.Dot(vToEnt) < 0.5071f)
				continue;*/

			if (fFovToEnemy < Vars::Visuals::FieldOfView.m_Var)
				continue;

			/*if (Vars::Visuals::SpyWarningVisibleOnly.m_Var)
			{
				CGameTrace Trace = {};
				CTraceFilterWorldAndPropsOnly Filter = {};

				Utils::Trace(vEnemyPos, vLocalPos, MASK_SOLID, &Filter, &Trace);

				if (Trace.flFraction < 1.0f)
					continue;
			}*/

			m_vecPlayers.push_back(vEnemyPos);
		}
		if (m_vecPlayers.empty())
			return;

		for (const auto& Player : m_vecPlayers) {
			Color_t teamColor;
			if (!Vars::ESP::Main::EnableTeamEnemyColors.m_Var) {
				if (pLocal->GetTeamNum() == 2) {
					teamColor = Colors::TeamBlu;
				}
				else {
					teamColor = Colors::TeamRed;
				}
			}
			else {
				teamColor = Colors::Enemy;
			}
			DrawArrowTo(vLocalPos, Player, teamColor);
		}
	}
}