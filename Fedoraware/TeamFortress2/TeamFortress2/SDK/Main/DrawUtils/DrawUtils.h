#pragma once
#include "../../Interfaces/Interfaces.h"

struct ScreenSize_t {
	int w = 0, h = 0, c = 0;
	void Update();
};

inline ScreenSize_t g_ScreenSize;

struct Font_t {
	DWORD dwFont = 0x0;
	const char* szName = nullptr;
	int nTall = 0;
	int nWeight = 0;
	int nFlags = 0;
};

enum EFonts {
	FONT_ESP,
	//FONT_ESP_OUTLINED,
	FONT_ESP_NAME,
	//FONT_ESP_NAME_OUTLINED,
	FONT_ESP_COND,
	//FONT_ESP_COND_OUTLINED,
	FONT_ESP_PICKUPS,
	//FONT_ESP_PICKUPS_OUTLINED,
	FONT_MENU,
	FONT_INDICATORS,
	FONT_IMGUI,
	FONT_OSRS,

	FONT_ENUM_SIZE // End of the font enum
};

enum EStringAlign {
	ALIGN_DEFAULT,
	ALIGN_CENTER,
	ALIGN_CENTERVERTICAL,
	ALIGN_CENTERHORIZONTAL,
	ALIGN_REVERSE
};

class CDraw {
	std::array<Font_t, FONT_ENUM_SIZE> m_Fonts{};

public:
	std::unordered_map<uint64, int> m_mapAvatars{};

	void RemakeFonts();
	void ReloadFonts();
	void String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign& align, const char* str, ...);
	void String(const size_t& font_idx, int x, int y, const Color_t& clr, const EStringAlign& align, const wchar_t* str, ...);
	void Line(int x, int y, int x1, int y1, const Color_t& clr);
	void DrawTexturedPolygon(int count, Vertex_t* vertices, const Color_t& clr);
	void DrawFilledTriangle(const std::array<Vec2, 3>& points, const Color_t& clr);
	void DrawOutlinedTriangle(const std::array<Vec2, 3>& points, const Color_t& clr);
	void Rect(int x, int y, int w, int h, const Color_t& clr);
	void RectOverlay(int x, int y, int w, int h, float bwidthp, const Color_t& clr, const Color_t& overlay_clr, bool horizontal);
	void OutlinedRect(int x, int y, int w, int h, const Color_t& clr);
	void GradientRect(int x, int y, int x1, int y1, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal);
	void GradientRectWH(int x, int y, int w, int h, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal);
	void OutlinedGradientBar(int x, int y, int w, int h, float bwidthp, const Color_t& top_clr, const Color_t& bottom_clr, const Color_t& overlay_clr, bool horizontal);
	void GradientRectA(int x, int y, int x1, int y1, const Color_t& top_clr, const Color_t& bottom_clr, bool horizontal);
	void OutlinedCircle(int x, int y, float radius, int segments, const Color_t& clr);
	void FilledCircle(int x, int y, int radius, int segments, Color_t clr);
	void CornerRect(int x, int y, int w, int h, int _x, int _y, const Color_t& color);
	void Texture(int x, int y, int w, int h, const Color_t& clr, int nIndex);
	CHudTexture* GetIcon(const char* szIcon, int eIconFormat = 0);
	int CreateTextureFromArray(const unsigned char* rgba, int w, int h);
	void DrawHudTexture(float x0, float y0, float s0, const CHudTexture* texture, Color_t col0);
	void DrawHudTextureByName(float x0, float y0, float s0, const char* textureName, Color_t col0);
	void Avatar(int x, int y, int w, int h, uint32 nFriendID);
	void RoundedBoxStatic(int x, int y, int w, int h, int radius, const Color_t& col);
	float EaseOut(float start, float end, float speed);
	float EaseIn(float start, float end, float speed);
	float Linear(float start, float end, float speed);
	bool Timer();
	void ClearAvatarCache();

	const Font_t& GetFont(size_t idx) { return m_Fonts[idx]; }
};

inline CDraw g_Draw;
