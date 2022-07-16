#include "DrawUtils.h"

#include "../../../SDK/Includes/icons.h"

void ScreenSize_t::Update()
{
	I::Engine->GetScreenSize(this->w, this->h);
	this->c = (this->w / 2);
}

void Draw_t::InitFonts(const std::vector<Font_t>& fonts)
{
	for (const auto& Font : fonts)
		m_vecFonts.push_back(Font);

	if (!m_vecFonts.empty())
		ReloadFonts();
}

void Draw_t::RemakeFonts(const std::vector<Font_t>& fonts) {
	m_vecFonts.clear();

	for (const auto& Font : fonts)
		m_vecFonts.push_back(Font);

	if (!m_vecFonts.empty())
		ReloadFonts();
}


void Draw_t::ReloadFonts()
{
	for (auto& v : m_vecFonts)
	{
		v.dwFont = I::Surface->CreateFont();
		I::Surface->SetFontGlyphSet(v.dwFont, v.szName, v.nTall, v.nWeight, 0, 0, v.nFlags);
	}
}

void Draw_t::String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign& align, const char* str, ...)
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
		I::Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
		x -= (w / 2);
		y -= (h / 2);
		break;
	}
	case ALIGN_CENTERVERTICAL: {
		int w = 0, h = 0;
		I::Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
		y -= (h / 2);
		break;
	}
	case ALIGN_CENTERHORIZONTAL: {
		int w = 0, h = 0;
		I::Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
		x -= (w / 2);
		break;
	}
	case ALIGN_REVERSE: {
		int w = 0, h = 0;
		I::Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
		x -= (w);
		break;
	}
	}

	I::Surface->SetTextPos(x, y);
	I::Surface->SetTextFont(m_vecFonts.at(font_idx).dwFont);
	I::Surface->SetTextColor(clr.r, clr.g, clr.b, clr.a);
	I::Surface->PrintText(wstr, wcslen(wstr));
}

void Draw_t::String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign& align, const wchar_t* str, ...)
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
		I::Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
		x -= (w / 2);
		y -= (h / 2);
		break;
	}
	case ALIGN_CENTERVERTICAL: {
		int w = 0, h = 0;
		I::Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
		y -= (h / 2);
		break;
	}
	case ALIGN_CENTERHORIZONTAL: {
		int w = 0, h = 0;
		I::Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
		x -= (w / 2);
		break;
	}
	case ALIGN_REVERSE: {
		int w = 0, h = 0;
		I::Surface->GetTextSize(m_vecFonts.at(font_idx).dwFont, wstr, w, h);
		x -= (w);
		break;
	}
	}

	I::Surface->SetTextPos(x, y);
	I::Surface->SetTextFont(m_vecFonts.at(font_idx).dwFont);
	I::Surface->SetTextColor(clr.r, clr.g, clr.b, clr.a);
	I::Surface->PrintText(wstr, wcslen(wstr));
}

void Draw_t::Line(int x, int y, int x1, int y1, const Color_t& clr)
{
	I::Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	I::Surface->DrawLine(x, y, x1, y1);
}

void Draw_t::DrawTexturedPolygon(int count, Vertex_t* vertices, const Color_t& clr)
{
	static int iTexture = I::Surface->CreateNewTextureID(true);

	I::Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	I::Surface->DrawSetTexture(iTexture);

	I::Surface->DrawTexturedPoly(count, vertices);
}

void Draw_t::DrawFilledTriangle(std::array<Vec2, 3>points, const Color_t& clr)
{
	std::array<Vertex_t, 3>vertices{ Vertex_t(points.at(0)), Vertex_t(points.at(1)), Vertex_t(points.at(2)) };
	DrawTexturedPolygon(3, vertices.data(), clr);
}

void Draw_t::DrawOutlinedTriangle(std::array<Vec2, 3>points, const Color_t& clr)
{
	Line(points.at(0).x, points.at(0).y, points.at(1).x, points.at(1).y, clr);
	Line(points.at(1).x, points.at(1).y, points.at(2).x, points.at(2).y, clr);
	Line(points.at(2).x, points.at(2).y, points.at(0).x, points.at(0).y, clr);
}

void Draw_t::Rect(int x, int y, int w, int h, const Color_t& clr)
{
	I::Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	I::Surface->DrawFilledRect(x, y, x + w, y + h);
}

void Draw_t::OutlinedRect(int x, int y, int w, int h, const Color_t& clr)
{
	I::Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	I::Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Draw_t::GradientRect(int x, int y, int x1, int y1, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal)
{
	I::Surface->SetDrawColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
	I::Surface->DrawFilledRectFade(x, y, x1, y1, 255, 255, horizontal);
	I::Surface->SetDrawColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
	I::Surface->DrawFilledRectFade(x, y, x1, y1, 0, 255, horizontal);
}

// above but does alpha better (imo)
void Draw_t::GradientRectA(int x, int y, int x1, int y1, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal)
{
	I::Surface->SetDrawColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
	I::Surface->DrawFilledRectFade(x, y, x1, y1, top_clr.a, bottom_clr.a, horizontal);
	I::Surface->SetDrawColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
	I::Surface->DrawFilledRectFade(x, y, x1, y1, top_clr.a, bottom_clr.a, horizontal);
}

void Draw_t::GradientRectWH(int x, int y, int w, int h, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal)
{
	I::Surface->SetDrawColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
	I::Surface->DrawFilledRectFade(x, y, x + w, y + h, 255, 255, horizontal);
	I::Surface->SetDrawColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
	I::Surface->DrawFilledRectFade(x, y, x + w, y + h, 0, 255, horizontal);
}

void Draw_t::OutlinedGradientBar(int x, int y, int w, int h, float bwidthp, const Color_t& top_clr, const Color_t& bottom_clr, const Color_t& outline_clr, bool horizontal)
{
	OutlinedRect(x - 1, y + 1 - (h + 2), w + 2, h + 2, outline_clr);
	GradientRectWH(x, y - h, w, h, bottom_clr, top_clr, horizontal);
	Rect(x, y - h, w, h * (1.0f - bwidthp), { 17, 24, 26, 255 });
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

void Draw_t::FilledCircle(const int x, const int y, const int radius, const int segments, const Color_t clr)
{
	CUtlVector<Vertex_t> vecVertixes;

	const float flStep = (6.28318530718f / static_cast<float>(segments));

	for (float n = 0.0f; n < 6.28318530718f; n += flStep)
		vecVertixes.AddToTail(Vertex_t({ (static_cast<float>(radius) * cos(n) + x), (static_cast<float>(radius) * sinf(n) + y) }, { 0.0f, 0.0f }));

	if (vecVertixes.Count() > 0)
	{
		I::Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
		I::Surface->DrawTexturedPoly(segments, vecVertixes.Base());
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
		nTexture = I::Surface->CreateNewTextureID();
		I::Surface->DrawSetTextureFile(nTexture, ICONS::TEXTURE[nIndex].c_str(), false, true);
		ICONS::ID[nIndex] = nTexture;
	}

	I::Surface->SetDrawColor(clr.r, clr.g, clr.b, clr.a);
	I::Surface->DrawSetTexture(nTexture);
	I::Surface->DrawTexturedRect(x, y, w, h);
}

//E8 ? ? ? ? 80 7F 3C 00
// Thanks myzarfin
CHudTexture* Draw_t::GetIcon(const char* szIcon, int eIconFormat /* = 0*/)
{
	using fn = CHudTexture * (__stdcall*)(const char*, int);
	static fn GetIconFn = reinterpret_cast<fn>(g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 83 7D 0C ? 56"));
	return GetIconFn(szIcon, eIconFormat);
}

int Draw_t::CreateTextureFromArray(const unsigned char* rgba, int w, int h)
{
	int nTextureIdOut = I::Surface->CreateNewTextureID(true);
	I::Surface->DrawSetTextureRGBAEx(nTextureIdOut, rgba, w, h, IMAGE_FORMAT_RGBA8888);
	return nTextureIdOut;
}

void Draw_t::DrawHudTexture(float x0, float y0, float s0, CHudTexture* texture, Color_t col0)
{
	if (!texture)
		return;

	if (texture->bRenderUsingFont)
	{

		I::Surface->DrawSetTextFont(texture->hFont);
		I::Surface->SetTextColor(col0.r, col0.g, col0.b, col0.a);
		I::Surface->SetTextPos(x0, y0);
		I::Surface->DrawUnicodeChar(texture->cCharacterInFont);
	}
	else if (texture->textureId != -1)
	{
		I::Surface->DrawSetTexture(texture->textureId);
		I::Surface->SetDrawColor(col0.r, col0.g, col0.b, col0.a);
		I::Surface->DrawTexturedSubRect(x0, y0, x0 + (texture->rc.right - texture->rc.left) * s0, y0 + (texture->rc.bottom - texture->rc.top) * s0, texture->texCoords[0], texture->texCoords[1], texture->texCoords[2], texture->texCoords[3]);
	}
}

void Draw_t::DrawHudTextureByName(float x0, float y0, float s0, const char* textureName, Color_t col0)
{
	CHudTexture* pIcon = GetIcon(textureName, 0);

	if (!pIcon)
		return;

	if (pIcon->bRenderUsingFont)
	{

		I::Surface->DrawSetTextFont(pIcon->hFont);
		I::Surface->SetTextColor(col0.r, col0.g, col0.b, col0.a);
		I::Surface->SetTextPos(x0, y0);
		I::Surface->DrawUnicodeChar(pIcon->cCharacterInFont);
	}
	else if (pIcon->textureId != -1)
	{
		I::Surface->DrawSetTexture(pIcon->textureId);
		I::Surface->SetDrawColor(col0.r, col0.g, col0.b, col0.a);
		I::Surface->DrawTexturedSubRect(x0, y0, x0 + (pIcon->rc.right - pIcon->rc.left) * s0, y0 + (pIcon->rc.bottom - pIcon->rc.top) * s0, pIcon->texCoords[0], pIcon->texCoords[1], pIcon->texCoords[2], pIcon->texCoords[3]);
	}
}

void Draw_t::Avatar(const int x, const int y, const int w, const int h, const uint32 nFriendID)
{
	if (const uint64 nID = static_cast<uint64>(nFriendID + 0x0110000100000000))
	{
		if (m_mapAvatars.find(nID) != m_mapAvatars.end())
		{
			I::Surface->SetDrawColor(255, 255, 255, 255);
			I::Surface->DrawSetTexture(m_mapAvatars[nID]);
			I::Surface->DrawTexturedRect(x, y, w, h);
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
					const int nTextureID = I::Surface->CreateNewTextureID(true);
					if (I::Surface->IsTextureIDValid(nTextureID))
					{
						I::Surface->DrawSetTextureRGBA(nTextureID, pData, w, h, 0, false);
						m_mapAvatars[nID] = nTextureID;
					}
				}

				free(pData);
			}
		}
	}
}

void Draw_t::RoundedBoxStatic(const int x, const int y, const int w, const int h, const int radius, const Color_t& col)
{
	Vertex_t roundsquare[64];

	for (int i = 0; i < 4; i++)
	{
		int _x = x + ((i < 2) ? (w - radius) : radius);
		int _y = y + ((i % 3) ? (h - radius) : radius);

		float a = 90.f * i;

		for (int j = 0; j < 16; j++)
		{
			float _a = DEG2RAD(a + j * 6.f);

			roundsquare[(i * 16) + j] = Vertex_t(Vector2D(_x + radius * sin(_a), _y - radius * cos(_a)));
		}
	}

	I::Surface->SetDrawColor(col.r, col.g, col.b, col.a);
	I::Surface->DrawTexturedPoly(64, roundsquare);
}



void Draw_t::ClearAvatarCache()
{
	for (auto Avatar : m_mapAvatars)
	{
		I::Surface->DeleteTextureByID(Avatar.second);
		I::Surface->DestroyTextureID(Avatar.second);
	}

	m_mapAvatars.clear();
}

float Draw_t::EaseOut(float start, float end, float speed)
{
	if (start > end || speed <= 1)
		return end;

	if (Timer())
	{
		if (start < end)
			return start * speed;
		return end;
	}
	return start;
}

float Draw_t::EaseIn(float start, float end, float speed)
{
	if (start < end || speed >= 1)
		return end;

	if (Timer())
	{
		if (start > end)
			return start * speed;
		return end;
	}
	return start;
}

float Draw_t::Linear(float start, float end, float speed)
{
	if (start < end)
		return start + speed;
	return end;
}

bool Draw_t::Timer() // This is to make sure that the animations don't get calculated in a split-second
{
	int t = clock();
	static int i = 0;

	if (t > i)
	{
		i += 1;
		return true;
	}
	return false;
}