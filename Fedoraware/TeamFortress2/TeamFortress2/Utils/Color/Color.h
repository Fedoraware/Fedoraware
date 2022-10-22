#pragma once

#include <string>
#include <sstream>

#include "../Math/Math.h"

#define DEVELOPER_BUILD

using byte = unsigned char;

struct Color_t
{
	byte r = 0, g = 0, b = 0, a = 0;
	[[nodiscard]] std::string to_hex() const
	{
		std::stringstream ss;
		ss << "\x7";
		ss << std::hex << (static_cast<int>(r) << 16 | static_cast<int>(g) << 8 | static_cast<int>(b));
		return ss.str();
	}

	[[nodiscard]] std::string to_hex_alpha() const
	{
		std::stringstream ss;
		ss << "\x8";
		ss << std::hex << (static_cast<int>(r) << 16 | static_cast<int>(g) << 8 | static_cast<int>(b));
		ss << std::hex << static_cast<int>(a);
		return ss.str();
	}
};

struct Gradient_t
{
	Color_t startColour = { 0,0,0,255 };
	Color_t endColour = { 0,0,0,255 };
};

// TODO: Move this shit to a file that isn't this
struct Chams_t
{
	bool	showObstructed = false;
	int		drawMaterial = 0;	// 1/shaded, 2/shiny, 3/flat, 4/wfshaded, 5/wfshiny, 6/wfflat, 7/fresnel, 8/brick
	int		overlayType = 0;
	bool	chamsActive = true;	// start all chams as active because I don't trust this cheat.
	bool	rainbow = false;
	bool	overlayPulse = false;
	bool	overlayRainbow = false;
	float	overlayIntensity = 1;
	Color_t fresnelBase = { 0,0,0,255 };
	Color_t colour = { 255, 255, 255, 255 };
	Color_t overlayColour = { 255, 255, 255, 255 };
	std::string customMaterial = "None";
};

struct DragBox_t
{
	int x = 100;
	int y = 100;
	int w = 80;
	int h = 30;
	int c = 140;
	bool update = true;
};

namespace Color
{
	inline float TOFLOAT(byte x)
	{
		return (static_cast<float>(x) / 255.0f);
	}

	inline Vec3 TOVEC3(const Color_t& x)
	{
		return Vec3(TOFLOAT(x.r), TOFLOAT(x.g), TOFLOAT(x.b));
	}

	inline unsigned long TODWORD(const Color_t& x)
	{
		return (static_cast<unsigned long>(((x.r & 0xFF) << 24) | ((x.g & 0xFF) << 16) | ((x.b & 0xFF) << 8) | (x.a & 0xFF)));
	}
}