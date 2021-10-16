#pragma once

#include "Color/Color.h"
#include "Interface/Interface.h"
#include "VFunc/VFunc.h"
#include "SEOHook/SEOHook.h"
#include "Pattern/Pattern.h"
#include "UtlVector/UtlVector.h"
#include "Hash/Hash.h"
#include "XorStr/XorStr.h"

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

namespace Utils
{
	inline uintptr_t GetVFuncPtr(void* pBaseClass, unsigned int nIndex)
	{
		return static_cast<uintptr_t>((*static_cast<int**>(pBaseClass))[nIndex]);
	}
}