#include "SpyWarning.h"
#include "../Vars.h"

bool CSpyWarning::ShouldRun(CBaseEntity* pLocal)
{
	if (!Vars::Visuals::SpyWarning.m_Var || g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	if (!pLocal->IsAlive() || pLocal->IsStunned())
		return false;

	if (pLocal->IsInBumperKart() || pLocal->IsAGhost())
		return false;

	return true;
}

void CSpyWarning::DrawArrowTo(const Vec3& vecFromPos, const Vec3& vecToPos)
{
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

	const float x1 = (20.0f + 5.0f) * xrot;
	const float y1 = (20.0f + 5.0f) * yrot;
	const float x2 = (20.0f + 15.0f) * xrot;
	const float y2 = (20.0f + 15.0f) * yrot;

	constexpr float arrow_angle = DEG2RAD(90.0f);
	constexpr float arrow_lenght = 10.0f;

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
	float cy = static_cast<float>((g_ScreenSize.h / 2) - 100);

	g_Draw.Line(cx + x2, cy + y2, cx + left.x, cy + left.y, { 255, 255, 255, 255 });
	g_Draw.Line(cx + x2, cy + y2, cx + right.x, cy + right.y, { 255, 255, 255, 255 });

	float fMap = std::clamp(MapFloat(vecFromPos.DistTo(vecToPos), 400.0f, 200.0f, 0.0f, 1.0f), 0.0f, 1.0f);
	Color_t HeatColor = { 255, 0, 0, static_cast<byte>(fMap * 255.0f) };

	g_Draw.Line(cx + x2, cy + y2, cx + left.x, cy + left.y, HeatColor);
	g_Draw.Line(cx + x2, cy + y2, cx + right.x, cy + right.y, HeatColor);
}

void CSpyWarning::Run()
{
	static bool bAnnounce = false;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!ShouldRun(pLocal))
			return;

		Vec3 vLocalPos = pLocal->GetWorldSpaceCenter();

		m_vecSpies.clear();

		for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!pEnemy || !pEnemy->IsAlive() || pEnemy->GetClassNum() != CLASS_SPY || pEnemy->IsCloaked() || pEnemy->IsAGhost())
				continue;

			if (Vars::Visuals::SpyWarningIgnoreFriends.m_Var && g_EntityCache.Friends[pEnemy->GetIndex()])
				continue;

			Vec3 vEnemyPos = pEnemy->GetWorldSpaceCenter();

			if (vLocalPos.DistTo(vEnemyPos) > 400.0f)
				continue;

			Vec3 vAngleToEnemy = Math::CalcAngle(vLocalPos, vEnemyPos);
			float fFovToEnemy = Math::CalcFov(pLocal->GetEyeAngles(), vAngleToEnemy);

			if (fFovToEnemy < 70.0f)
				continue;

			if (Vars::Visuals::SpyWarningVisibleOnly.m_Var)
			{
				CGameTrace Trace = {};
				CTraceFilterWorldAndPropsOnly Filter = {};

				Utils::Trace(vEnemyPos, vLocalPos, MASK_SOLID, &Filter, &Trace);

				if (Trace.flFraction < 1.0f)
					continue;
			}

			m_vecSpies.push_back(vEnemyPos);
		}

		if (Vars::Visuals::SpyWarningAnnounce.m_Var)
		{
			static bool bOldEmpty = m_vecSpies.empty();

			if (m_vecSpies.empty() != bOldEmpty)
			{
				if (!m_vecSpies.empty())
					g_Interfaces.Engine->ClientCmd_Unrestricted(_("voicemenu 1 1"));

				bOldEmpty = m_vecSpies.empty();
			}
		}

		if (Vars::Visuals::SpyWarningStyle.m_Var == 0)
		{
			if (m_vecSpies.empty())
				return;

			static const int size = 40;
			g_Draw.Texture(((g_ScreenSize.w / 2) - (size / 2)), (((g_ScreenSize.h / 2) - 100) - (size / 2)), size, size, Colors::White, 8);

			for (const auto& Spy : m_vecSpies)
				DrawArrowTo(vLocalPos, Spy);
		}

		else
		{
			static float flTimer = 0.0f;
			static const float flDuration = 0.1f; //100ms
			static bool bOldEmpty = m_vecSpies.empty();

			if (m_vecSpies.empty() != bOldEmpty)
			{
				if (!m_vecSpies.empty())
					flTimer = g_Interfaces.GlobalVars->curtime;

				bOldEmpty = m_vecSpies.empty();
			}

			if (flTimer)
			{
				float flAlpha = Math::RemapValClamped(g_Interfaces.GlobalVars->curtime - flTimer, 0.0f, flDuration, 0.5f, 0.0f);

				if (flAlpha <= 0.0f)
					flTimer = 0.0f;

				g_Draw.Rect(0, 0, g_ScreenSize.w, g_ScreenSize.h, { 255, 0, 0, static_cast<byte>(flAlpha * 255.0f) });
			}
		}
	}
}