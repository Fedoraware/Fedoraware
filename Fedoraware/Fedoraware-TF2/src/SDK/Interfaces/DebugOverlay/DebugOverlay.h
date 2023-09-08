#pragma once
#include "../../Includes/Includes.h"

#define PRINTF_FORMAT_STRING 

class OverlayText_t
{
public:
	OverlayText_t()
	{
		nextOverlayText = 0;
		bUseOrigin = false;
		lineOffset = 0;
		flXPos = 0;
		flYPos = 0;
		text[0] = 0;
		m_flEndTime = 0.0f;
		m_nServerCount = -1;
		m_nCreationTick = -1;
		r = g = b = a = 255;
	}

	bool			IsDead();
	void			SetEndTime(float duration);

	Vec3			origin;
	bool			bUseOrigin;
	int				lineOffset;
	float			flXPos;
	float			flYPos;
	char			text[512];
	float			m_flEndTime;			// When does this text go away
	int				m_nCreationTick;		// If > 0, show only one server frame
	int				m_nServerCount;			// compare server spawn count to remove stale overlays
	int				r;
	int				g;
	int				b;
	int				a;
	OverlayText_t* nextOverlayText;
};

struct CDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, PRINTF_FORMAT_STRING const char* format, ...) = 0;
	virtual void AddBoxOverlay(const Vec3& origin, const Vec3& mins, const Vec3& max, Vec3 const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddTriangleOverlay(const Vec3& p1, const Vec3& p2, const Vec3& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddLineOverlay(const Vec3& origin, const Vec3& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const Vec3& origin, float duration, PRINTF_FORMAT_STRING const char* format, ...) = 0;
	virtual void AddTextOverlay(const Vec3& origin, int line_offset, float duration, PRINTF_FORMAT_STRING const char* format, ...) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos, int line_offset, float flDuration, int r, int g, int b, int a, const char* text) = 0;
	virtual void AddSweptBoxOverlay(const Vec3& start, const Vec3& end, const Vec3& mins, const Vec3& max, const Vec3& angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddGridOverlay(const Vec3& origin) = 0;
	virtual int ScreenPosition(const Vec3& point, Vec3& screen) = 0;
	virtual int ScreenPosition(float flXPos, float flYPos, Vec3& screen) = 0;

	virtual OverlayText_t* GetFirst(void) = 0;
	virtual OverlayText_t* GetNext(OverlayText_t* current) = 0;
	virtual void ClearDeadOverlays(void) = 0;
	virtual void ClearAllOverlays() = 0;

	virtual void AddTextOverlayRGB(const Vec3& origin, int line_offset, float duration, float r, float g, float b, float alpha, PRINTF_FORMAT_STRING const char* format, ...) = 0;
	virtual void AddTextOverlayRGB(const Vec3& origin, int line_offset, float duration, int r, int g, int b, int a, PRINTF_FORMAT_STRING const char* format, ...) = 0;

	virtual void Unknown_1() = 0;
	virtual void AddLineOverlayAlpha(const Vec3& origin, const Vec3& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddBoxOverlay2(const Vec3& origin, const Vec3& mins, const Vec3& max, Vec3 const& orientation, const Color_t& faceColor, const Color_t& edgeColor, float duration) = 0;

private:
	inline void AddTextOverlay(const Vec3& origin, int line_offset, float duration, int r, int g, int b, int a, PRINTF_FORMAT_STRING const char* format, ...) {} /* catch improper use of bad interface. Needed because '0' duration can be resolved by compiler to NULL format string (i.e., compiles but calls wrong function) */
};

#define VENGINE_DEBUGOVERLAY_INTERFACE_VERSION	"VDebugOverlay003"
