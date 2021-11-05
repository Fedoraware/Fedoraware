#pragma once
#include "../../Interfaces/Interfaces.h"

struct ScreenSize_t
{
	int w = 0, h = 0, c = 0;
	void Update();
};

inline ScreenSize_t g_ScreenSize;

struct Font_t
{
	DWORD dwFont = 0x0;
	const char* szName = 0;
	int nTall = 0;
	int nWeight = 0;
	int nFlags = 0;
};

enum EStringAlign
{
	ALIGN_DEFAULT,
	ALIGN_CENTER,
	ALIGN_CENTERVERTICAL,
	ALIGN_CENTERHORIZONTAL
};

struct Draw_t
{
	std::vector<Font_t>   m_vecFonts = { };
	std::map<uint64, int> m_mapAvatars = { };

	void InitFonts(const std::vector<Font_t>& fonts);
	void ReloadFonts();
	void String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign& align, const char* str, ...);
	void String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign& align, const wchar_t* str, ...);
	void Line(int x, int y, int x1, int y1, const Color_t& clr);
	void Rect(int x, int y, int w, int h, const Color_t& clr);
	void OutlinedRect(int x, int y, int w, int h, const Color_t& clr);
	void GradientRect(int x, int y, int x1, int y1, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal);
	void OutlinedCircle(int x, int y, float radius, int segments, const Color_t& clr);
	void CornerRect(int x, int y, int w, int h, int _x, int _y, const Color_t& color);
	void Texture(int x, int y, int w, int h, const Color_t& clr, int nIndex);
	void Avatar(const int x, const int y, const int w, const int h, const uint32 nFriendID);

	void ClearAvatarCache();
};

enum EFonts
{
	FONT_ESP,
	FONT_ESP_OUTLINED,

	FONT_ESP_NAME,
	FONT_ESP_NAME_OUTLINED,

	FONT_ESP_COND,
	FONT_ESP_COND_OUTLINED,

	FONT_ESP_PICKUPS,
	FONT_ESP_PICKUPS_OUTLINED,

	FONT_MENU,
	FONT_DEBUG,

	FONT_INDICATORS
};

inline Draw_t g_Draw;