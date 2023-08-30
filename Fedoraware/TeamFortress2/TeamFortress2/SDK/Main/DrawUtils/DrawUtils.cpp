#include "DrawUtils.h"

#include <ranges>

#include "../../Features/Vars.h"
#include "../../../SDK/Includes/icons.h"

void ScreenSize_t::Update()
{
	I::EngineClient->GetScreenSize(this->w, this->h);
	this->c = (this->w / 2);
}

void CDraw::RemakeFonts()
{
	m_Fonts[FONT_ESP] = {0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.Value, Vars::Fonts::FONT_ESP::nWeight.Value, Vars::Fonts::FONT_ESP::nFlags.Value};
	m_Fonts[FONT_ESP_NAME] = {0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.Value, Vars::Fonts::FONT_ESP_NAME::nWeight.Value, Vars::Fonts::FONT_ESP_NAME::nFlags.Value};
	m_Fonts[FONT_ESP_COND] = {0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.Value, Vars::Fonts::FONT_ESP_COND::nWeight.Value, Vars::Fonts::FONT_ESP_COND::nFlags.Value};
	m_Fonts[FONT_ESP_PICKUPS] = {0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value};
	m_Fonts[FONT_MENU] = {0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.Value, Vars::Fonts::FONT_MENU::nWeight.Value, Vars::Fonts::FONT_MENU::nFlags.Value};
	m_Fonts[FONT_INDICATORS] = {0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.Value, Vars::Fonts::FONT_INDICATORS::nWeight.Value, Vars::Fonts::FONT_INDICATORS::nFlags.Value};
	m_Fonts[FONT_IMGUI] = {0x0, "Verdana", 18, 800, FONTFLAG_ANTIALIAS};
	m_Fonts[FONT_OSRS] = {0x0, "Verdana", 12, 800, FONTFLAG_DROPSHADOW};

	ReloadFonts();
}


void CDraw::ReloadFonts()
{
	for (auto& v : m_Fonts)
	{
		v.dwFont = I::VGuiSurface->CreateFont();
		I::VGuiSurface->SetFontGlyphSet(v.dwFont, v.szName, v.nTall, v.nWeight, 0, 0, v.nFlags);
	}
}

void CDraw::String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign& align, const char* str, ...)
{
	if (str == nullptr)
	{
		return;
	}

	va_list va_alist;
	char cbuffer[1024] = {'\0'};
	wchar_t wstr[1024] = {'\0'};

	va_start(va_alist, str);
	vsprintf_s(cbuffer, str, va_alist);
	va_end(va_alist);

	wsprintfW(wstr, L"%hs", cbuffer);

	const auto& font = m_Fonts.at(font_idx);
	const auto dwFont = font.dwFont;

	switch (align)
	{
	case ALIGN_DEFAULT: break;
	case ALIGN_CENTER:
		{
			int w = 0, h = 0;
			I::VGuiSurface->GetTextSize(dwFont, wstr, w, h);
			x -= (w / 2);
			y -= (h / 2);
			break;
		}
	case ALIGN_CENTERVERTICAL:
		{
			int w = 0, h = 0;
			I::VGuiSurface->GetTextSize(dwFont, wstr, w, h);
			y -= (h / 2);
			break;
		}
	case ALIGN_CENTERHORIZONTAL:
		{
			int w = 0, h = 0;
			I::VGuiSurface->GetTextSize(dwFont, wstr, w, h);
			x -= (w / 2);
			break;
		}
	case ALIGN_REVERSE:
		{
			int w = 0, h = 0;
			I::VGuiSurface->GetTextSize(dwFont, wstr, w, h);
			x -= (w);
			break;
		}
	}

	I::VGuiSurface->DrawSetTextPos(x, y);
	I::VGuiSurface->DrawSetTextFont(dwFont);
	I::VGuiSurface->DrawSetTextColor(clr.r, clr.g, clr.b, clr.a);
	I::VGuiSurface->DrawPrintText(wstr, wcslen(wstr));
}

void CDraw::String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign& align, const wchar_t* str, ...)
{
	if (str == nullptr)
	{
		return;
	}

	va_list va_alist;
	wchar_t wstr[1024] = {'\0'};

	va_start(va_alist, str);
	vswprintf_s(wstr, str, va_alist);
	va_end(va_alist);

	const auto& font = m_Fonts.at(font_idx);
	const auto dwFont = font.dwFont;

	switch (align)
	{
	case ALIGN_DEFAULT: break;
	case ALIGN_CENTER:
		{
			int w = 0, h = 0;
			I::VGuiSurface->GetTextSize(dwFont, wstr, w, h);
			x -= (w / 2);
			y -= (h / 2);
			break;
		}
	case ALIGN_CENTERVERTICAL:
		{
			int w = 0, h = 0;
			I::VGuiSurface->GetTextSize(dwFont, wstr, w, h);
			y -= (h / 2);
			break;
		}
	case ALIGN_CENTERHORIZONTAL:
		{
			int w = 0, h = 0;
			I::VGuiSurface->GetTextSize(dwFont, wstr, w, h);
			x -= (w / 2);
			break;
		}
	case ALIGN_REVERSE:
		{
			int w = 0, h = 0;
			I::VGuiSurface->GetTextSize(dwFont, wstr, w, h);
			x -= (w);
			break;
		}
	}

	I::VGuiSurface->DrawSetTextPos(x, y);
	I::VGuiSurface->DrawSetTextFont(dwFont);
	I::VGuiSurface->DrawSetTextColor(clr.r, clr.g, clr.b, clr.a);
	I::VGuiSurface->DrawPrintText(wstr, wcslen(wstr));
}

void CDraw::Line(int x, int y, int x1, int y1, const Color_t& clr)
{
	I::VGuiSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::VGuiSurface->DrawLine(x, y, x1, y1);
}

void CDraw::DrawTexturedPolygon(int count, Vertex_t* vertices, const Color_t& clr)
{
	static int iTexture = I::VGuiSurface->CreateNewTextureID(true);

	I::VGuiSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::VGuiSurface->DrawSetTexture(iTexture);

	I::VGuiSurface->DrawTexturedPolygon(count, vertices);
}

void CDraw::DrawFilledTriangle(const std::array<Vec2, 3>& points, const Color_t& clr)
{
	std::array<Vertex_t, 3> vertices{Vertex_t(points.at(0)), Vertex_t(points.at(1)), Vertex_t(points.at(2))};
	DrawTexturedPolygon(3, vertices.data(), clr);
}

void CDraw::DrawOutlinedTriangle(const std::array<Vec2, 3>& points, const Color_t& clr)
{
	Line(points.at(0).x, points.at(0).y, points.at(1).x, points.at(1).y, clr);
	Line(points.at(1).x, points.at(1).y, points.at(2).x, points.at(2).y, clr);
	Line(points.at(2).x, points.at(2).y, points.at(0).x, points.at(0).y, clr);
}

void CDraw::Rect(int x, int y, int w, int h, const Color_t& clr)
{
	I::VGuiSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
}

void CDraw::RectOverlay(int x, int y, int w, int h, float bwidthp, const Color_t& clr, const Color_t& outline_clr, bool horizontal)
{
	OutlinedRect(x - 1, y + 1 - (h + 2), w + 2, h + 2, outline_clr);
	GradientRectWH(x, y - h, w, h, clr, clr, horizontal);
	Rect(x, y - h, w, h * (1.0f - bwidthp), {17, 24, 26, 255});
}

void CDraw::OutlinedRect(int x, int y, int w, int h, const Color_t& clr)
{
	I::VGuiSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::VGuiSurface->DrawOutlinedRect(x, y, x + w, y + h);
}

void CDraw::GradientRect(int x, int y, int x1, int y1, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal)
{
	I::VGuiSurface->DrawSetColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
	I::VGuiSurface->DrawFilledRectFade(x, y, x1, y1, 255, 255, horizontal);
	I::VGuiSurface->DrawSetColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
	I::VGuiSurface->DrawFilledRectFade(x, y, x1, y1, 0, 255, horizontal);
}

// above but does alpha better (imo)
void CDraw::GradientRectA(int x, int y, int x1, int y1, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal)
{
	I::VGuiSurface->DrawSetColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
	I::VGuiSurface->DrawFilledRectFade(x, y, x1, y1, top_clr.a, bottom_clr.a, horizontal);
	I::VGuiSurface->DrawSetColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
	I::VGuiSurface->DrawFilledRectFade(x, y, x1, y1, top_clr.a, bottom_clr.a, horizontal);
}

void CDraw::GradientRectWH(int x, int y, int w, int h, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal)
{
	I::VGuiSurface->DrawSetColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
	I::VGuiSurface->DrawFilledRectFade(x, y, x + w, y + h, 255, 255, horizontal);
	I::VGuiSurface->DrawSetColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
	I::VGuiSurface->DrawFilledRectFade(x, y, x + w, y + h, 0, 255, horizontal);
}

void CDraw::OutlinedGradientBar(int x, int y, int w, int h, float bwidthp, const Color_t& top_clr, const Color_t& bottom_clr, const Color_t& outline_clr, bool horizontal)
{
	OutlinedRect(x - 1, y + 1 - (h + 2), w + 2, h + 2, outline_clr);
	GradientRectWH(x, y - h, w, h, bottom_clr, top_clr, horizontal);
	Rect(x, y - h, w, h * (1.0f - bwidthp), {17, 24, 26, 255});
}

void CDraw::OutlinedCircle(int x, int y, float radius, int segments, const Color_t& clr)
{
	const float step = PI * 2.0 / segments;

	for (float a = 0; a < (PI * 2.0); a += step)
	{
		const float x1 = radius * cos(a) + x;
		const float y1 = radius * sin(a) + y;
		const float x2 = radius * cos(a + step) + x;
		const float y2 = radius * sin(a + step) + y;
		Line(x1, y1, x2, y2, clr);
	}
}

void CDraw::FilledCircle(const int x, const int y, const int radius, const int segments, const Color_t clr)
{
	CUtlVector<Vertex_t> vecVertixes;

	const float flStep = (6.28318530718f / static_cast<float>(segments));

	for (float n = 0.0f; n < 6.28318530718f; n += flStep)
	{
		vecVertixes.AddToTail(Vertex_t({(static_cast<float>(radius) * cos(n) + x), (static_cast<float>(radius) * sinf(n) + y)}, {0.0f, 0.0f}));
	}

	if (vecVertixes.Count() > 0)
	{
		I::VGuiSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
		I::VGuiSurface->DrawTexturedPolygon(segments, vecVertixes.Base());
	}
}

void CDraw::CornerRect(int x, int y, int w, int h, int _x, int _y, const Color_t& color)
{
	Line(x, y, x + (w / _x), y, color);
	Line(x, y, x, y + (h / _y), color);

	Line(x + w, y, x + w - (w / _x), y, color);
	Line(x + w, y, x + w, y + (h / _y), color);

	Line(x, y + h, x + (w / _x), y + h, color);
	Line(x, y + h, x, y + h - (h / _y), color);

	Line(x + w, y + h, x + w - (w / _x), y + h, color);
	Line(x + w, y + h, x + w, y + h - (h / _y), color);
}

void CDraw::Texture(int x, int y, int w, int h, const Color_t& clr, int nIndex)
{
	static int nTexture = 0;

	if (ICONS::ID[nIndex] != ICONS::UNDEFINED)
	{
		nTexture = ICONS::ID[nIndex];
	}
	else
	{
		nTexture = I::VGuiSurface->CreateNewTextureID();
		I::VGuiSurface->DrawSetTextureFile(nTexture, ICONS::TEXTURE[nIndex].c_str(), false, true);
		ICONS::ID[nIndex] = nTexture;
	}

	I::VGuiSurface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
	I::VGuiSurface->DrawSetTexture(nTexture);
	I::VGuiSurface->DrawTexturedRect(x, y, w, h);
}

//E8 ? ? ? ? 80 7F 3C 00
// Thanks myzarfin
CHudTexture* CDraw::GetIcon(const char* szIcon, int eIconFormat /* = 0*/)
{
	using fn = CHudTexture * (__stdcall*)(const char*, int);
	static auto GetIconFn = reinterpret_cast<fn>(g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 83 7D 0C ? 56"));
	return GetIconFn(szIcon, eIconFormat);
}

int CDraw::CreateTextureFromArray(const unsigned char* rgba, int w, int h)
{
	const int nTextureIdOut = I::VGuiSurface->CreateNewTextureID(true);
	I::VGuiSurface->DrawSetTextureRGBAEx(nTextureIdOut, rgba, w, h, IMAGE_FORMAT_RGBA8888);
	return nTextureIdOut;
}

void CDraw::DrawHudTexture(float x0, float y0, float s0, const CHudTexture* texture, Color_t col0)
{
	if (!texture)
	{
		return;
	}

	if (texture->bRenderUsingFont)
	{
		I::VGuiSurface->DrawSetTextFont(texture->hFont);
		I::VGuiSurface->DrawSetTextColor(col0.r, col0.g, col0.b, col0.a);
		I::VGuiSurface->DrawSetTextPos(x0, y0);
		I::VGuiSurface->DrawUnicodeChar(texture->cCharacterInFont);
	}
	else if (texture->textureId != -1)
	{
		I::VGuiSurface->DrawSetTexture(texture->textureId);
		I::VGuiSurface->DrawSetColor(col0.r, col0.g, col0.b, col0.a);
		I::VGuiSurface->DrawTexturedSubRect(x0, y0, x0 + (texture->rc.right - texture->rc.left) * s0, y0 + (texture->rc.bottom - texture->rc.top) * s0, texture->texCoords[0], texture->texCoords[1],
		                                    texture->texCoords[2], texture->texCoords[3]);
	}
}

void CDraw::DrawHudTextureByName(float x0, float y0, float s0, const char* textureName, Color_t col0)
{
	const CHudTexture* pIcon = GetIcon(textureName, 0);

	if (!pIcon)
	{
		return;
	}

	if (pIcon->bRenderUsingFont)
	{
		I::VGuiSurface->DrawSetTextFont(pIcon->hFont);
		I::VGuiSurface->DrawSetTextColor(col0.r, col0.g, col0.b, col0.a);
		I::VGuiSurface->DrawSetTextPos(x0, y0);
		I::VGuiSurface->DrawUnicodeChar(pIcon->cCharacterInFont);
	}
	else if (pIcon->textureId != -1)
	{
		I::VGuiSurface->DrawSetTexture(pIcon->textureId);
		I::VGuiSurface->DrawSetColor(col0.r, col0.g, col0.b, col0.a);
		I::VGuiSurface->DrawTexturedSubRect(x0, y0, x0 + (pIcon->rc.right - pIcon->rc.left) * s0, y0 + (pIcon->rc.bottom - pIcon->rc.top) * s0, pIcon->texCoords[0], pIcon->texCoords[1],
		                                    pIcon->texCoords[2], pIcon->texCoords[3]);
	}
}

void CDraw::Avatar(const int x, const int y, const int w, const int h, const uint32 nFriendID)
{
	if (const auto nID = static_cast<uint64>(nFriendID + 0x0110000100000000))
	{
		if (m_Avatars.contains(nID))
		{
			// The avatar has been cached
			I::VGuiSurface->DrawSetColor(255, 255, 255, 255);
			I::VGuiSurface->DrawSetTexture(m_Avatars[nID]);
			I::VGuiSurface->DrawTexturedRect(x, y, w, h);
		}
		else
		{
			// Retrieve the avatar
			const int nAvatar = g_SteamInterfaces.Friends->GetMediumFriendAvatar(CSteamID(nID));

			uint32 newW = 0, newH = 0;
			if (g_SteamInterfaces.Utils->GetImageSize(nAvatar, &newW, &newH))
			{
				const size_t nSize = 4 * newW * newH * sizeof(uint8);
				auto* pData = static_cast<uint8*>(std::malloc(nSize));
				if (!pData) { return; }

				if (g_SteamInterfaces.Utils->GetImageRGBA(nAvatar, pData, static_cast<int>(nSize)))
				{
					const int nTextureID = I::VGuiSurface->CreateNewTextureID(true);
					if (I::VGuiSurface->IsTextureIDValid(nTextureID))
					{
						I::VGuiSurface->DrawSetTextureRGBA(nTextureID, pData, newW, newH, 0, false);
						m_Avatars[nID] = nTextureID;
					}
				}

				std::free(pData);
			}
		}
	}
}

void CDraw::RoundedBoxStatic(const int x, const int y, const int w, const int h, const int radius, const Color_t& col)
{
	Vertex_t roundsquare[64];

	for (int i = 0; i < 4; i++)
	{
		const int _x = x + ((i < 2) ? (w - radius) : radius);
		const int _y = y + ((i % 3) ? (h - radius) : radius);

		const float a = 90.f * i;

		for (int j = 0; j < 16; j++)
		{
			const float _a = DEG2RAD(a + j * 6.f);

			roundsquare[(i * 16) + j] = Vertex_t(Vector2D(_x + radius * sin(_a), _y - radius * cos(_a)));
		}
	}

	I::VGuiSurface->DrawSetColor(col.r, col.g, col.b, col.a);
	I::VGuiSurface->DrawTexturedPolygon(64, roundsquare);
}


void CDraw::ClearAvatarCache()
{
	for (const auto& id : m_Avatars | std::views::values)
	{
		I::VGuiSurface->DeleteTextureByID(id);
		I::VGuiSurface->DestroyTextureID(id);
	}

	m_Avatars.clear();
}

float CDraw::EaseOut(float start, float end, float speed)
{
	if (start > end || speed <= 1)
	{
		return end;
	}

	if (Timer())
	{
		if (start < end)
		{
			return start * speed;
		}
		return end;
	}
	return start;
}

float CDraw::EaseIn(float start, float end, float speed)
{
	if (start < end || speed >= 1)
	{
		return end;
	}

	if (Timer())
	{
		if (start > end)
		{
			return start * speed;
		}
		return end;
	}
	return start;
}

float CDraw::Linear(float start, float end, float speed)
{
	if (start < end)
	{
		return start + speed;
	}
	return end;
}

bool CDraw::Timer() // This is to make sure that the animations don't get calculated in a split-second
{
	const int t = clock();
	static int i = 0;

	if (t > i)
	{
		i += 1;
		return true;
	}
	return false;
}
