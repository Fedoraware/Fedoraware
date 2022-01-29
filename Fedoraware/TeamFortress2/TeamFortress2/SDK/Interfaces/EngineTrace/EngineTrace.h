#pragma once
#include "../../Includes/Includes.h"

#ifndef ENGINETRACE_H
#define ENGINETRACE_H
class CTraceListData;

class IEntityEnumerator
{
public:
	virtual bool EnumEntity(IHandleEntity* pHandleEntity) = 0;
};

class CPhysCollide;

class CEngineTrace
{
public:
	virtual int GetPointContents(const Vector& vecAbsPosition, IHandleEntity** ppEntity = NULL) = 0;
	virtual int GetPointContents_Collideable(ICollideable* pCollide, const Vector& vecAbsPosition) = 0;
	virtual void ClipRayToEntity(const Ray_t& ray, unsigned int fMask, IHandleEntity* pEnt, CGameTrace* pTrace) = 0;
	virtual void ClipRayToCollideable(const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, CGameTrace* pTrace) = 0;
	virtual void TraceRay(const Ray_t& ray, unsigned int fMask, CTraceFilter* pTraceFilter, CGameTrace* pTrace) = 0;
};

#define VENGINE_TRACE_CLIENT_INTERFACE_VERSION "EngineTraceClient003"
#endif