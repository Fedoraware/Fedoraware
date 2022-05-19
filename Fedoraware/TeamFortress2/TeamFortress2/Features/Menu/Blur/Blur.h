#pragma once

#include <array>
#include <d3d9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../ImGui/imgui.h"

template <std::size_t N>
constexpr auto DecodeBase85(const char(&input)[N]) noexcept
{
	std::array<char, N * 4 / 5> out{};

	constexpr auto decode85Byte = [](char c) constexpr -> unsigned int { return c >= '\\' ? c - 36 : c - 35; };

	for (std::size_t i = 0, j = 0; i < N - 1; i += 5, j += 4) {
		unsigned int tmp = decode85Byte(input[i]) + 85 * (decode85Byte(input[i + 1]) + 85 * (decode85Byte(input[i + 2]) + 85 * (decode85Byte(input[i + 3]) + 85 * decode85Byte(input[i + 4]))));
		out[j] = ((tmp >> 0) & 0xFF); out[j + 1] = ((tmp >> 8) & 0xFF); out[j + 2] = ((tmp >> 16) & 0xFF); out[j + 3] = ((tmp >> 24) & 0xFF);
	}

	return out;
}

inline constexpr auto BLUR_X = DecodeBase85(
	"%/P:vP>$(#>T$<8?####Q$###%/P:v%####?####$&###J$###h####&####$####t#########.$###%####$####:$########tn=j=$8HlEQ2TuY3l:$#%),##$#########0g'WC"
	"`-.:CVjSuY&5>##%),##$#########C,.33UnSc;'AViF6JrEH<Sk:0+bVe=K&&PDlf1eGdfX1F$*fUCs'el1K>,C5AH3I3b48(#$QUV$)%XHVd;#K7#####X/'.7`7r'7$QUV$*%XHV"
	"d:i[7bmhf6##########D5YY#NSi.L,nHS[D5YY#_9r:Q0=XHVi>uu#^XF0LdfIl[[BA`V&5YY#W.]/Lpu$GV+>uu#PYr.LOV%JLou$GV&5YY#Q`%/Lpv*PV(>uu#Sf./L5hJcLdfIl["
	"(>uu#Rf./L4_/GLdfIl[&5YY#Y.]/Lqu$GV+>uu#RYr.LQV%JLou$GV&5YY#S`%/Lpv*PV(>uu#Uf./L7hJcLdfIl[(>uu#Tf./L6_/GLdfIl[i>uu#_XF0L4_/GL[BA`Vi>uu#`XF0L"
	"5_/GL[BA`Vi>uu#aXF0L6_/GL[BA`Vi>uu#bXF0L7_/GL[BA`V+>uu#W(S/L5_/GLpw0YV+G:;$W(S/L3_/GLpx6cV5_/GL+G:;$V(S/L4_/GLpw0YV5_/GL+G:;$V(S/L7_/GLqv*PV"
	"4_/GL+G:;$U(S/L6_/GLqv*PV4_/GL&5YY#fqF0L3_/GL#),##");

inline constexpr auto BLUR_Y = DecodeBase85(
	"%/P:vP>$(#>T$<8?####Q$###%/P:v%####?####$&###J$###h####&####$####t#########.$###%####$####:$########tn=j=$8HlEQ2TuY3l:$#%),##$#########0g'WC"
	"Qk;nDhpF/#&5>##%),##$#########C,.33UnSc;'AViF6JrEH<Sk:0+bVe=K&&PDlf1eGdfX1F$*fUCs'el1K>,C5AH3I3b48(#$QUV$)%XHVd;#K7NSi.LX/'.7`7r'7$QUV$*%XHV"
	"d:i[7bmhf6##########D5YY#NSi.L,nHS[D5YY#_9r:Q0=XHVi>uu#^XF0LdfIl[[BA`V&5YY#W.]/Lpu$GV+>uu#PYr.LOV%JLou$GV&5YY#Q`%/LP].JL&5YY#PYr.Lpv*PV(>uu#"
	"Rf./L4_/GLdfIl[&5YY#QYr.L)[-S[+G:;$R`%/Lou$GVOV%JL)]3][&5YY#Y.]/Lqu$GV+>uu#Sl7/LQV%JLou$GV&5YY#S`%/LP_:]L&5YY#RYr.Lpv*PV(>uu#Tf./L6_/GLdfIl["
	"&5YY#SYr.L)[-S[+G:;$T`%/Lou$GVQV%JL)]3][i>uu#_XF0L4_/GL[BA`Vi>uu#`XF0L5_/GL[BA`Vi>uu#aXF0L6_/GL[BA`Vi>uu#bXF0L7_/GL[BA`V+>uu#V(S/L4_/GLpw0YV"
	"+G:;$V(S/L3_/GLpx6cV4_/GL+G:;$V(S/L5_/GLpw0YV4_/GL+G:;$V(S/L6_/GLqv*PV4_/GL+G:;$U(S/L7_/GLqv*PV4_/GL&5YY#fqF0L3_/GL#),##");

class CBlur {
public:
	void DrawBackgroundBlur(ImDrawList* drawList, IDirect3DDevice9* device, int strength = 8);
};

inline CBlur g_Blur;