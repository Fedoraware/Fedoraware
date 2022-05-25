#pragma once

#include "Color/Color.h"
#include "Interface/Interface.h"
#include "VFunc/VFunc.h"
#include "Pattern/Pattern.h"
#include "UtlVector/UtlVector.h"
#include "Hash/Hash.h"
#include "XorStr/XorStr.h"
#include "KeyHelper/KeyHelper.hpp"
#include "Timer/Timer.hpp"
#include "BytePatch/BytePatch.hpp"

#include "Hash/FNV1A.h"
#include "TFM/tfm.h"

#include <Windows.h>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <map>
#include <chrono>
#include <string>
#include <random>
#include <intrin.h>
#include <thread>
#include <array>

#undef min
#undef max

inline void Q_memcpy(void* dest, const void* src, int count)
{
	int i;
	if (((reinterpret_cast<long>(dest) | reinterpret_cast<long>(src) | count) & 3) == 0)
	{
		count >>= 2;
		for (i = 0; i < count; i++)
		{
			static_cast<int*>(dest)[i] = ((int*)src)[i];
		}
	}
	else
	{
		for (i = 0; i < count; i++)
		{
			static_cast<char*>(dest)[i] = ((char*)src)[i];
		}
	}
}

namespace Utils
{
	inline uintptr_t GetVFuncPtr(void* pBaseClass, unsigned int nIndex)
	{
		return static_cast<uintptr_t>((*static_cast<int**>(pBaseClass))[nIndex]);
	}

	inline void ReplaceSpecials(std::string& str) // thx cathook
	{
		int val;
		size_t c = 0;
		const size_t len = str.size();
		for (int i = 0; i + c < len; ++i)
		{
			str[i] = str[i + c];
			if (str[i] != '\\') { continue; }
			if (i + c + 1 == len) { break; }

			switch (str[i + c + 1])
			{
			// Several control characters
			case 'b':
				++c;
				str[i] = '\b';
				break;
			case 'n':
				++c;
				str[i] = '\n';
				break;
			case 'v':
				++c;
				str[i] = '\v';
				break;
			case 'r':
				++c;
				str[i] = '\r';
				break;
			case 't':
				++c;
				str[i] = '\t';
				break;
			case 'f':
				++c;
				str[i] = '\f';
				break;
			case 'a':
				++c;
				str[i] = '\a';
				break;
			// Write escaped escape character as is
			case '\\':
				++c;
				break;
			// Convert specified value from HEX
			case 'x':
				if (i + c + 4 > len)
				{
					continue;
				}
				std::sscanf(&str[i + c + 2], "%02X", &val);
				c += 3;
				str[i] = val;
				break;
			// Convert from unicode
			case 'u':
				if (i + c + 6 > len)
				{
					continue;
				}
			// 1. Scan 16bit HEX value
				std::sscanf(&str[i + c + 2], "%04X", &val);
				c += 5;
			// 2. Convert value to UTF-8
				if (val <= 0x7F)
				{
					str[i] = val;
				}
				else if (val <= 0x7FF)
				{
					str[i] = 0xC0 | ((val >> 6) & 0x1F);
					str[i + 1] = 0x80 | (val & 0x3F);
					++i;
					--c;
				}
				else
				{
					str[i] = 0xE0 | ((val >> 12) & 0xF);
					str[i + 1] = 0x80 | ((val >> 6) & 0x3F);
					str[i + 2] = 0x80 | (val & 0x3F);
					i += 2;
					c -= 2;
				}
				break;
			}
		}
		str.resize(len - c);
	}

	inline bool CompareFloat(float a, float b, float epsilon = 1.0e-5f)
	{
		return (fabs(a - b) <= epsilon * std::max(fabs(a), fabs(b)));
	}

	inline bool StartsWith(const char* a, const char* b)
	{
		if (strncmp(a, b, strlen(b)) == 0) { return true; }
		return false;
	}

	inline std::vector<std::string> SplitString(const std::string& pString, const std::string& pDelimeter)
	{
		std::vector<std::string> strings;

		std::string::size_type pos = 0;
		std::string::size_type prev = 0;
		while ((pos = pString.find(pDelimeter, prev)) != std::string::npos)
		{
			strings.push_back(pString.substr(prev, pos - prev));
			prev = pos + 1;
		}

		strings.push_back(pString.substr(prev));
		return strings;
	}

	inline Vec3 GetRotatedPosition(Vec3 vOrigin, const float flRotation, const float flDistance)
	{
		const auto rad = DEG2RAD(flRotation);
		vOrigin.x += cosf(rad) * flDistance;
		vOrigin.y += sinf(rad) * flDistance;

		return vOrigin;

	}
}
