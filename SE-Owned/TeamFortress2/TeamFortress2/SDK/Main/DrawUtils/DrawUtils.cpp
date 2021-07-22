#include "DrawUtils.h"

#include "../../../SDK/Includes/icons.h"

void ScreenSize_t::Update()
{
	g_Interfaces.Engine->GetScreenSize(this->w, this->h);
	this->c = (this->w / 2);
}

void Draw_t::InitFonts(const std::vector<Font_t>& fonts)
{
	for (const auto& Font : fonts)
		m_vecFonts.push_back(Font);

	if (!m_vecFonts.empty())
		ReloadFonts();
}

void Draw_t::ReloadFonts()
{
	for (auto& v : m_vecFonts)
	{
		v.dwFont = g_Interfaces.Surface->CreateFont();
		g_Interfaces.Surface->SetFontGlyphSet(v.dwFont, v.szName, v.nTall, v.nWeight, 0, 0, v.nFlags);
	}
}

void Draw_t::String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign &align, const char* str, ...)
{
	if (str == 0)
		return;

	va_list va_alist;
	char cbuffer[1024] = { '\0' };
	wchar_t wstr[1024] = { '\0' };

	va_start(va_alist, str);
	vsprintf_s(cbuffer, str, va_alist);
	va_end(va_alist);

	wsprintfW(wstr, L"%hs", cbuffer);

	switch (align)
	{
		case ALIGN_DEFAULT: break;
		case ALIGN_CENTER: {
			int w = 0, h = 0;
			g_Interfaces.Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
			x -= (w / 2);
			y -= (h / 2);
			break;
		}
		case ALIGN_CENTERVERTICAL: {
			int w = 0, h = 0;
			g_Interfaces.Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
			y -= (h / 2);
			break;
		}
		case ALIGN_CENTERHORIZONTAL: {
			int w = 0, h = 0;
			g_Interfaces.Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
			x -= (w / 2);
			break;
		}
	}

	g_Interfaces.Surface->SetTextPos(x, y);
	g_Interfaces.Surface->SetTextFont(m_vecFonts.at(font_idx).dwFont);
	g_Interfaces.Surface->SetTextColor(clr.r, clr.g, clr.b, clr.a);
	g_Interfaces.Surface->PrintText(wstr, wcslen(wstr));
}

void Draw_t::String(const size_t &font_idx, int x, int y, const Color_t &clr, const EStringAlign &align, const wchar_t *str, ...)
{
	if (str == 0)
		return;

	va_list va_alist;
	wchar_t wstr[1024] = { '\0' };

	va_start(va_alist, str);
	vswprintf_s(wstr, str, va_alist);
	va_end(va_alist);

	//wsprintfW(wstr, L"%ls", cbuffer);

	switch (align)
	{
		case ALIGN_DEFAULT: break;
		case ALIGN_CENTER: {
			int w = 0, h = 0;
			g_Interfaces.Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
			x -= (w / 2);
			y -= (h / 2);
			break;
		}
		case ALIGN_CENTERVERTICAL: {
			int w = 0, h = 0;
			g_Interfaces.Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
			y -= (h / 2);
			break;
		}
		case ALIGN_CENTERHORIZONTAL: {
			int w = 0, h = 0;
			g_Interfaces.Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
			x -= (w / 2);
			break;
		}
	}

	g_Interfaces.Surface->SetTextPos(x, y);
	g_Interfaces.Surface->SetTextFont(m_vecFonts.at(font_idx).dwFont);
	g_Interfaces.Surface->SetTextColor(clr.r, clr.g, clr.b, clr.a);
	g_Interfaces.Surface->PrintText(wstr, wcslen(wstr));
}

void Draw_t::Line(int x, int y, int x1, int y1, const Color_t& clr)
{
	g_Interfaces.Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	g_Interfaces.Surface->DrawLine(x, y, x1, y1);
}

void Draw_t::Rect(int x, int y, int w, int h, const Color_t& clr)
{
	g_Interfaces.Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	g_Interfaces.Surface->DrawFilledRect(x, y, x + w, y + h);
}

void Draw_t::OutlinedRect(int x, int y, int w, int h, const Color_t& clr)
{
	g_Interfaces.Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	g_Interfaces.Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Draw_t::GradientRect(int x, int y, int x1, int y1, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal)
{
	g_Interfaces.Surface->SetDrawColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
	g_Interfaces.Surface->DrawFilledRectFade(x, y, x1, y1, 255, 255, horizontal);
	g_Interfaces.Surface->SetDrawColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
	g_Interfaces.Surface->DrawFilledRectFade(x, y, x1, y1, 0, 255, horizontal);
}

void Draw_t::OutlinedCircle(int x, int y, float radius, int segments, const Color_t& clr)
{
	float Step = PI * 2.0 / segments;

	for (float a = 0; a < (PI * 2.0); a += Step) 
	{
		float x1 = radius * cos(a) + x;
		float y1 = radius * sin(a) + y;
		float x2 = radius * cos(a + Step) + x;
		float y2 = radius * sin(a + Step) + y;
		Line(x1, y1, x2, y2, clr);
	}
}

void Draw_t::CornerRect(int x, int y, int w, int h, int _x, int _y, const Color_t& color)
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

void Draw_t::Texture(int x, int y, int w, int h, const Color_t& clr, int nIndex)
{
	static int nTexture = 0;

	if (ICONS::ID[nIndex] != ICONS::UNDEFINED)
	{
		nTexture = ICONS::ID[nIndex];
	}
	else
	{
		nTexture = g_Interfaces.Surface->CreateNewTextureID();
		g_Interfaces.Surface->DrawSetTextureFile(nTexture, ICONS::TEXTURE[nIndex].c_str(), false, true);
		ICONS::ID[nIndex] = nTexture;
	}

	g_Interfaces.Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	g_Interfaces.Surface->DrawSetTexture(nTexture);
	g_Interfaces.Surface->DrawTexturedRect(x, y, w, h);
}

void Draw_t::Avatar(const int x, const int y, const int w, const int h, const uint32 nFriendID)
{
	if (const uint64 nID = static_cast<uint64>(nFriendID + 0x0110000100000000))
	{
		if (m_mapAvatars.find(nID) != m_mapAvatars.end())
		{
			g_Interfaces.Surface->SetDrawColor(255, 255, 255, 255);
			g_Interfaces.Surface->DrawSetTexture(m_mapAvatars[nID]);
			g_Interfaces.Surface->DrawTexturedRect(x, y, w, h);
		}
		else
		{
			const int nAvatar = g_SteamInterfaces.Friends015->GetMediumFriendAvatar(CSteamID(nID));

			uint32 w = { }, h = { };
			if (g_SteamInterfaces.Utils007->GetImageSize(nAvatar, &w, &h))
			{
				const int nSize = static_cast<int>(4 * w * h * sizeof(uint8));
				uint8* pData = reinterpret_cast<uint8*>(malloc(nSize));

				if (g_SteamInterfaces.Utils007->GetImageRGBA(nAvatar, pData, nSize))
				{
					const int nTextureID = g_Interfaces.Surface->CreateNewTextureID(true);
					if (g_Interfaces.Surface->IsTextureIDValid(nTextureID))
					{
						g_Interfaces.Surface->DrawSetTextureRGBA(nTextureID, pData, w, h, 0, false);
						m_mapAvatars[nID] = nTextureID;
					}
				}

				free(pData);
			}
		}
	}
}

void Draw_t::ClearAvatarCache()
{
	for (auto Avatar : m_mapAvatars)
	{
		g_Interfaces.Surface->DeleteTextureByID(Avatar.second);
		g_Interfaces.Surface->DestroyTextureID(Avatar.second);
	}

	m_mapAvatars.clear();
}