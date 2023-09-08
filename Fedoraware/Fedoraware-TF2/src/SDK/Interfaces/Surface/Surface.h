#pragma once
#include "../../Includes/Includes.h"

#undef CreateFont
#undef PlaySound

enum FontDrawType_t
{
	// Use the "additive" value from the scheme file
	FONT_DRAW_DEFAULT = 0,

	// Overrides
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,

	FONT_DRAW_TYPE_COUNT = 2,
};

class IImage;
class Image;
class Point;
class IHTML;
class IHTMLEvents;
class IVguiMatInfo
{

};


using HCursor = unsigned long;
using HTexture = unsigned long;
using HFont = unsigned long;
using VPANEL = unsigned int;

struct CharRenderInfo
{
	int				x, y;
	Vertex_t* verts;
	int				textureId;
	int				abcA;
	int				abcB;
	int				abcC;
	int				fontTall;
	HFont			currentFont;
	FontDrawType_t	drawType;
	wchar_t			ch;
	bool			valid;
	bool			shouldclip;
};




struct IntRect
{
	int x0;
	int y0;
	int x1;
	int y1;
};


enum SurfaceFeature_e
{
	ANTIALIASED_FONTS = 1,
	DROPSHADOW_FONTS = 2,
	ESCAPE_KEY = 3,
	OPENING_NEW_HTML_WINDOWS = 4,
	FRAME_MINIMIZE_MAXIMIZE = 5,
	OUTLINE_FONTS = 6,
	DIRECT_HWND_RENDER = 7,
};

class CSurface
{
public:
	// What the fuck goes before this one? Anything I add crashes me
	void DrawSetColor(int r, int g, int b, int a)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 11)(this, r, g, b, a);
	}
	void DrawSetColor(Color_t col)
	{
		return DrawSetColor(col.r, col.g, col.b, col.a);
	}
	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 12)(this, x0, y0, x1, y1);
	}
	void DrawFilledRectArray(IntRect* pRects, int numRects)
	{
		return GetVFunc<void(__thiscall*)(PVOID, IntRect* pRects, int numRects)>(this, 13)(this, pRects, numRects);
	}
	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 14)(this, x0, y0, x1, y1);
	}
	void DrawLine(int x, int y, int x1, int y1)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 15)(this, x, y, x1, y1);
	}
	void DrawPolyLine(int* px, int* py, int numPoints)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int*, int*, int)>(this, 16)(this, px, py, numPoints);
	}
	void DrawSetTextFont(HFont font)
	{
		return GetVFunc<void(__thiscall*)(PVOID, HFont)>(this, 17)(this, font);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 19)(this, r, g, b, a);
	}
	void DrawSetTextColor(Color_t col)
	{
		DrawSetTextColor(col.r, col.g, col.b, col.a);
	}
	void DrawSetTextPos(int x, int y)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int)>(this, 20)(this, x, y);
	}
	void DrawGetTextPos(int& x, int& y)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int&, int&)>(this, 20)(this, x, y);
	}
	void DrawPrintText(const wchar_t* text, int text_len, FontDrawType_t drawType = FONT_DRAW_DEFAULT)
	{
		return GetVFunc<void(__thiscall*)(PVOID, const wchar_t*, int, int)>(this, 22)(this, text, text_len, 0);
	}
	void DrawUnicodeChar(wchar_t wch, FontDrawType_t drawType = FONT_DRAW_DEFAULT)
	{
		return GetVFunc<void(__thiscall*)(PVOID, wchar_t, FontDrawType_t)>(this, 23)(this, wch, drawType);
	}
	void DrawFlushText()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 24)(this);
	}
	IHTML* CreateHTMLWindow(IHTMLEvents* events, VPANEL context)
	{
		return GetVFunc<IHTML* (__thiscall*)(PVOID, IHTMLEvents* events, VPANEL context)>(this, 25)(this, events, context);
	}
	void PaintHTMLWindow(IHTML* htmlwin)
	{
		return GetVFunc<void(__thiscall*)(PVOID, IHTML* htmlwin)>(this, 26)(this, htmlwin);
	}
	void DeleteHTMLWindow(IHTML* htmlwin)
	{
		return GetVFunc<void(__thiscall*)(PVOID, IHTML* htmlwin)>(this, 27)(this, htmlwin);
	}
	int	DrawGetTextureId(char const* filename)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, char const*)>(this, 28)(this, filename);
	}
	bool DrawGetTextureFile(int id, char* filename, int maxlen)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, int, char*, int)>(this, 29)(this, id, filename, maxlen);
	};
	void DrawSetTextureFile(int id, const char* filename, int hardwareFilter, bool forceReload)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, const char*, int, bool)>(this, 30)(this, id, filename, hardwareFilter, forceReload);
	};
	void DrawSetTextureRGBA(int id, unsigned char const* rgba, int wide, int tall, int hardwareFilter = 0, bool forceReload = false)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, unsigned char const*, int, int, int, bool)>(this, 31)(this, id, rgba, wide, tall, hardwareFilter, forceReload);
	}
	void DrawSetTexture(int id)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int)>(this, 32)(this, id);
	}
	void DrawGetTextureSize(int id, int& wide, int& tall)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int&, int&)>(this, 33)(this, id, wide, tall);
	};
	void DrawTexturedRect(int x, int y, int w, int h)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 34)(this, x, y, x + w, y + h);
	}
	bool IsTextureIDValid(int id)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, int)>(this, 35)(this, id);
	}
	bool DeleteTextureByID(int id)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, int)>(this, 36)(this, id);
	}
	int CreateNewTextureID(bool procedural = true)
	{
		return GetVFunc<int(__thiscall*)(PVOID, bool)>(this, 37)(this, procedural);
	}
	void GetScreenSize(int& wide, int& tall)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int&, int&)>(this, 38)(this, wide, tall);
	}
	void SetAsTopMost(VPANEL panel, bool state)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool)>(this, 39)(this, panel, state);
	}
	void BringToFront(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 40)(this, panel);
	}
	void SetForegroundWindow(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 41)(this, panel);
	}
	void SetPanelVisible(VPANEL panel, bool state)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool)>(this, 42)(this, panel, state);
	}
	void SetMinimized(VPANEL panel, bool state)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool)>(this, 43)(this, panel, state);
	}
	void IsMinimized(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 44)(this, panel);
	}
	void FlashWindow(VPANEL panel, bool state)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool)>(this, 45)(this, panel, state);
	}
	void SetTitle(VPANEL panel, const wchar_t* title)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, const wchar_t*)>(this, 46)(this, panel, title);
	}
	void SetAsToolBar(VPANEL panel, bool state)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool)>(this, 47)(this, panel, state);
	}
	void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool, bool, bool, bool, bool)>(this, 48)(this, panel, minimised, showTaskbarIcon, disabled, mouseInput, kbInput);
	}
	void SwapBuffers(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 49)(this, panel);
	}
	void Invalidate(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 50)(this, panel);
	}
	void SetCursor(HCursor cursor)
	{
		return GetVFunc<void(__thiscall*)(PVOID, HCursor)>(this, 51)(this, cursor);
	}
	void SetCursorAlwaysVisible(bool b)
	{
		GetVFunc<void(__thiscall*)(PVOID, bool)>(this, 52)(this, b);
	}
	bool IsCursorVisible()
	{
		return GetVFunc<bool(__thiscall*)(PVOID)>(this, 53)(this);
	}
	void ApplyChanges()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 54)(this);
	}
	bool IsWithin(int x, int y)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, int, int)>(this, 55)(this, x, y);
	}
	bool HasFocus()
	{
		return GetVFunc<bool(__thiscall*)(PVOID)>(this, 56)(this);
	}
	bool SupportsFeature(SurfaceFeature_e feature)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, SurfaceFeature_e)>(this, 57)(this, feature);
	}
	void RestrictPaintToSinglePanel(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 58)(this, panel);
	}
	void SetModalPanel(VPANEL)
	{
		// Source moment
	}
	VPANEL GetModalPanel()
	{
		return GetVFunc<VPANEL(__thiscall*)(PVOID)>(this, 60)(this);
	}
	void UnlockCursor()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 61)(this);
	}
	void LockCursor()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 62)(this);
	}
	void SetTranslateExtendedKeys(bool state)
	{
		return GetVFunc<void(__thiscall*)(PVOID, bool)>(this, 63)(this, state);
	}
	VPANEL GetTopmostPopup()
	{
		return GetVFunc<VPANEL(__thiscall*)(PVOID)>(this, 64)(this);
	}
	void SetTopLevelFocus(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 65)(this, panel);
	}
	unsigned long CreateFont()
	{
		return GetVFunc<unsigned int(__thiscall*)(PVOID)>(this, 66)(this);
	}
	void SetFontGlyphSet(unsigned long& font, const char* windows_font_name, int tall, int weight, int blur, int scanlines, int flags)
	{
		return GetVFunc<void(__thiscall*)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int)>(this, 67)(this, font, windows_font_name, tall, weight, blur, scanlines, flags, 0, 0);
	}
	bool AddCustomFontFile(const char* fontName, const char* fontFileName)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, const char*, const char*)>(this, 68)(this, fontName, fontFileName);
	}
	int GetFontTall(HFont font)
	{
		return GetVFunc<int(__thiscall*)(PVOID, HFont)>(this, 69)(this, font);
	}
	int GetFontTallRequested(HFont font)
	{
		return GetVFunc<int(__thiscall*)(PVOID, HFont)>(this, 70)(this, font);
	}
	int GetFontAscent(HFont font, wchar_t wch)
	{
		return GetVFunc<int(__thiscall*)(PVOID, HFont, wchar_t)>(this, 71)(this, font, wch);
	}
	bool IsFontAdditive(HFont font)
	{
		return GetVFunc<int(__thiscall*)(PVOID, HFont)>(this, 72)(this, font);
	}
	void GetCharABCwide(HFont font, int ch, int& a, int& b, int& c)
	{
		return GetVFunc<void(__thiscall*)(PVOID, HFont, int, int&, int&, int&)>(this, 73)(this, font, ch, a, b, c);
	}
	int GetCharacterWidth(HFont font, int ch)
	{
		return GetVFunc<int(__thiscall*)(PVOID, HFont, int)>(this, 74)(this, font, ch);
	}
	void GetTextSize(unsigned long font, const wchar_t* text, int& wide, int& tall)
	{
		GetVFunc<void(__thiscall*)(PVOID, unsigned long, const wchar_t*, int&, int&)>(this, 75)(this, font, text, wide, tall);
	}
	VPANEL GetNotifyPanel()
	{
		return GetVFunc<VPANEL(__thiscall*)(PVOID)>(this, 76)(this);
	}
	void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char* text)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, HTexture, VPANEL, const char*)>(this, 77)(this, context, icon, panelToReceiveMessages, text);
	}
	void PlaySound(const char* szFile)
	{
		return GetVFunc<void(__thiscall*)(PVOID, const char*)>(this, 78)(this, szFile);
	}
	int GetPopupCount()
	{
		return GetVFunc<int(__thiscall*)(PVOID)>(this, 79)(this);
	}
	VPANEL GetPopup(int index)
	{
		return GetVFunc<VPANEL(__thiscall*)(PVOID, int)>(this, 80)(this, index);
	}
	bool ShouldPaintChildPanel(VPANEL childPanel)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, VPANEL)>(this, 81)(this, childPanel);
	}
	bool RecreateContext(VPANEL panel)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, VPANEL)>(this, 82)(this, panel);
	}
	void AddPanel(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 83)(this, panel);
	}
	void ReleasePanel(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 84)(this, panel);
	}
	void MovePopupToFront(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 85)(this, panel);
	}
	void MovePopupToBack(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 86)(this, panel);
	}
	void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool)>(this, 87)(this, panel, forceApplySchemeSettings);
	}
	void PaintTraverse(VPANEL panel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 88)(this, panel);
	}
	void EnableMouseCapture(VPANEL panel, bool state)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool)>(this, 89)(this, panel, state);
	}
	void GetWorkspaceBounds(int& x, int& y, int& wide, int& tall)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int&, int&, int&, int&)>(this, 90)(this, x, y, wide, tall);
	}
	void GetAbsoluteWindowBounds(int& x, int& y, int& wide, int& tall)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int&, int&, int&, int&)>(this, 91)(this, x, y, wide, tall);
	}
	void GetProportionalBase(int& width, int& height)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int&, int&)>(this, 92)(this, width, height);
	}
	void CalculateMouseVisible()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 93)(this);
	}
	bool NeedKBInput()
	{
		return GetVFunc<bool(__thiscall*)(PVOID)>(this, 94)(this);
	}
	bool HasCursorPosFunctions()
	{
		return GetVFunc<bool(__thiscall*)(PVOID)>(this, 95)(this);
	}
	void SurfaceGetCursorPos(int& x_out, int& y_out)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int&, int&)>(this, 96)(this, x_out, y_out);
	}
	void SurfaceSetCursorPos(int x, int y)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int)>(this, 97)(this, x, y);
	}
	void DrawTexturedLine(const Vertex_t& a, const Vertex_t& b)
	{
		return GetVFunc<void(__thiscall*)(PVOID, const Vertex_t&, const Vertex_t&)>(this, 98)(this, a, b);
	}
	void DrawOutlinedCircle(int x, int y, float radius, int segments)
	{
		GetVFunc<void(__thiscall*)(PVOID, int, int, float, int)>(this, 99)(this, x, y, radius, segments);
	}
	void DrawTexturedPolyLine(const Vertex_t* p, int n)
	{
		GetVFunc<void(__thiscall*)(PVOID, const Vertex_t* p, int n)>(this, 100)(this, p, n);
	}
	void DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1)
	{
		GetVFunc<void(__thiscall*)(PVOID, int, int, int, int, float, float, float, float)>(this, 101)(this, x0, y0, x1, y1, texs0, text0, texs1, text1);
	}
	void DrawTexturedPolygon(int n, Vertex_t* vertices, bool bClipVertices = true)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, Vertex_t*, bool)>(this, 102)(this, n, vertices, bClipVertices);
	}
	const wchar_t* GetTitle(VPANEL panel)
	{
		return GetVFunc<const wchar_t* (__thiscall*)(PVOID, VPANEL)>(this, 103)(this, panel);
	}
	bool IsCursorLocked(void)
	{
		return GetVFunc<bool(__thiscall*)(PVOID)>(this, 104)(this);
	}
	void SetWorkspaceInsets(int left, int top, int right, int bottom)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 105)(this, left, top, right, bottom);
	}
	bool DrawGetUnicodeCharRenderInfo(wchar_t ch, CharRenderInfo& info)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, wchar_t, CharRenderInfo&)>(this, 106)(this, ch, info);
	}
	void DrawRenderCharFromInfo(const CharRenderInfo& info)
	{
		return GetVFunc<void(__thiscall*)(PVOID, const CharRenderInfo&)>(this, 107)(this, info);
	}
	void DrawSetAlphaMultiplier(float alpha)
	{
		return GetVFunc<void(__thiscall*)(PVOID, float)>(this, 108)(this, alpha);
	}
	float DrawGetAlphaMultiplier()
	{
		return GetVFunc<float(__thiscall*)(PVOID)>(this, 109)(this);
	}
	void SetAllowHTMLJavaScript(bool state)
	{
		return GetVFunc<void(__thiscall*)(PVOID, bool)>(this, 110)(this, state);
	}
	void OnScreenSizeChanged(int nOldWidth, int nOldHeight)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int)>(this, 111)(this, nOldWidth, nOldHeight);
	}
	HCursor CreateCursorFromFile(char const* curOrAniFile, char const* pPathID = 0)
	{
		return GetVFunc<HCursor(__thiscall*)(PVOID, char const* curOrAniFile, char const* pPathID)>(this, 112)(this, curOrAniFile, pPathID);
	}
	IVguiMatInfo* DrawGetTextureMatInfoFactory(int id)
	{
		return GetVFunc<IVguiMatInfo*(__thiscall*)(PVOID, int)>(this, 113)(this, id);
	}
	void PaintTraverseEx(VPANEL panel, bool paintPopups = false)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL, bool)>(this, 114)(this, panel, paintPopups);
	}
	float GetZPos()
	{
		return GetVFunc<float(__thiscall*)(PVOID)>(this, 115)(this);
	}
	void SetPanelForInput(VPANEL vpanel)
	{
		return GetVFunc<void(__thiscall*)(PVOID, VPANEL)>(this, 116)(this, vpanel);
	}
	void DrawFilledRectFastFade(int x0, int y0, int x1, int y1, int fadeStartPt, int fadeEndPt, unsigned int alpha0, unsigned int alpha1, bool bHorizontal)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int, int, int, unsigned int, unsigned int, bool)>(this, 117)(this, x0, y0, x1, y1, fadeStartPt, fadeEndPt, alpha0, alpha1, bHorizontal);;
	}
	void DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool horizontal)
	{
		GetVFunc<void(__thiscall*)(PVOID, int, int, int, int, unsigned int, unsigned int, bool)>(this, 118)(this, x0, y0, x1, y1, alpha0, alpha1, horizontal);
	}
	void DrawSetTextureRGBAEx(int id, const unsigned char* rgba, int wide, int tall, ImageFormat imageFormat)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int id, const unsigned char* rgba, int wide, int tall, ImageFormat imageFormat)>(this, 119)(this, id, rgba, wide, tall, imageFormat);
	}
	void DrawSetTextScale(float sx, float sy)
	{
		return GetVFunc<void(__thiscall*)(PVOID, float, float)>(this, 120)(this, sx, sy);
	}
	bool SetBitmapFontGlyphSet(HFont font, const char* windowsFontName, float scalex, float scaley, int flags)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, HFont, const char*, float, float, int)>(this, 121)(this, font, windowsFontName, scalex, scaley, flags);
	}
	bool AddBitmapFontFile(const char* fontFileName)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, const char*)>(this, 122)(this, fontFileName);
	}
	void SetBitmapFontName(const char* pName, const char* pFontFilename)
	{
		return GetVFunc<void(__thiscall*)(PVOID, const char*, const char*)>(this, 123)(this, pName, pFontFilename);
	}
	const char* GetBitmapFontName(const char* pName)
	{
		return GetVFunc<const char* (__thiscall*)(PVOID, const char*)>(this, 124)(this, pName);
	}
	void ClearTemporaryFontCache(void)
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 125)(this);
	}
	IImage* GetIconImageForFullPath(char const* pFullPath)
	{
		return GetVFunc<IImage* (__thiscall*)(PVOID, char const*)>(this, 126)(this, pFullPath);
	}
	void DrawUnicodeString(const wchar_t* pwString, FontDrawType_t drawType = FONT_DRAW_DEFAULT)
	{
		return GetVFunc<void(__thiscall*)(PVOID, const wchar_t*, FontDrawType_t)>(this, 127)(this, pwString, drawType);
	}
	void PrecacheFontCharacters(HFont font, const wchar_t* pCharacters)
	{
		return GetVFunc<void(__thiscall*)(PVOID, HFont, const wchar_t*)>(this, 128)(this, font, pCharacters);
	}
	const char* GetResolutionKey(void)
	{
		return GetVFunc<const char* (__thiscall*)(PVOID)>(this, 129)(this);
	}
	const char* GetFontName(HFont font)
	{
		return GetVFunc<const char* (__thiscall*)(PVOID, HFont)>(this, 130)(this, font);
	}
	const char* GetFontFamilyName(HFont font)
	{
		return GetVFunc<const char* (__thiscall*)(PVOID, HFont)>(this, 131)(this, font);
	}
	void GetKernedCharWidth(HFont font, wchar_t ch, wchar_t chBefore, wchar_t chAfter, float& wide, float& abcA)
	{
		return GetVFunc<void(__thiscall*)(PVOID, HFont, wchar_t, wchar_t, wchar_t, float&, float&)>(this, 132)(this, font, ch, chBefore, chAfter, wide, abcA);
	}
	bool ForceScreenSizeOverride(bool bState, int wide, int tall)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, bool, int, int)>(this, 133)(this, bState, wide, tall);
	}
	bool ForceScreenPosOffset(bool bState, int x, int y)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, bool, int, int)>(this, 134)(this, bState, x, y);
	}
	void OffsetAbsPos(int& x, int& y)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int)>(this, 135)(this, x, y);
	}
	void ResetFontCaches()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 136)(this);
	}
	int GetTextureNumFrames(int id)
	{
		return GetVFunc<int(__thiscall*)(PVOID, int)>(this, 137)(this, id);
	}
	void DrawSetTextureFrame(int id, int nFrame, unsigned int* pFrameCache)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, unsigned int*)>(this, 138)(this, id, nFrame, pFrameCache);
	}
	bool IsScreenSizeOverrideActive(void)
	{
		return GetVFunc<bool(__thiscall*)(PVOID)>(this, 139)(this);
	}
	bool IsScreenPosOverrideActive(void)
	{
		return GetVFunc<bool(__thiscall*)(PVOID)>(this, 140)(this);
	}
	void DestroyTextureID(int id)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int)>(this, 141)(this, id);
	}
	void DrawUpdateRegionTextureRGBA(int nTextureID, int x, int y, const unsigned char* pchData, int wide, int tall, ImageFormat imageFormat)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, const unsigned char*, int, int, ImageFormat)>(this, 142)(this, nTextureID, x, y, pchData, wide, tall, imageFormat);
	}
	bool BHTMLWindowNeedsPaint(IHTML* htmlwin)
	{
		return GetVFunc<bool(__thiscall*)(PVOID, IHTML*)>(this, 143)(this, htmlwin);
	}
	const char* GetWebkitHTMLUserAgentString()
	{
		return GetVFunc<const char* (__thiscall*)(PVOID)>(this, 144)(this);
	}
	void* Deprecated_AccessChromeHTMLController()
	{
		return GetVFunc<void* (__thiscall*)(PVOID)>(this, 145)(this);
	}
	void SetFullscreenViewport(int x, int y, int w, int h)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 146)(this, x, y, w, h);
	}
	void GetFullscreenViewport(int& x, int& y, int& w, int& h)
	{
		return GetVFunc<void(__thiscall*)(PVOID, int, int, int, int)>(this, 147)(this, x, y, w, h);
	}
	void PushFullscreenViewport()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 148)(this);
	}
	void PopFullscreenViewport()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 149)(this);
	}
	void SetSoftwareCursor(bool bUseSoftwareCursor)
	{
		return GetVFunc<void(__thiscall*)(PVOID, bool)>(this, 150)(this, bUseSoftwareCursor);
	}
	void PaintSoftwareCursor()
	{
		return GetVFunc<void(__thiscall*)(PVOID)>(this, 151)(this);
	}
};

#define VGUI_SURFACE_INTERFACE_VERSION "VGUI_Surface030"