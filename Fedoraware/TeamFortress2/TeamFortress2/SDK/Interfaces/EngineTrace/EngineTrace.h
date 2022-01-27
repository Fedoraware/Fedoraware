#pragma once
#include "../../Includes/Includes.h"

class CEngineTrace
{
public:
	void TraceRay(const Ray_t &Ray, unsigned int Mask, CTraceFilter *TraceFilter, CGameTrace *GameTrace)
	{
		typedef void(__thiscall *FN)(PVOID, const Ray_t &, unsigned int, CTraceFilter *, CGameTrace *);
		return GetVFunc<FN>(this, 4)(this, Ray, Mask, TraceFilter, GameTrace);
	}
};

#define VENGINE_TRACE_CLIENT_INTERFACE_VERSION "EngineTraceClient003"