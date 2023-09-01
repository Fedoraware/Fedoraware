#pragma once
#include "../BaseEntity/BaseEntity.h"

class CTraceFilterHitscan : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity(void* pEntityHandle, int nContentsMask);
	virtual ETraceType GetTraceType() const;
	CBaseEntity* pSkip = nullptr;
};

class CTraceFilterWorldAndPropsOnly : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity(void* pEntityHandle, int nContentsMask);
	virtual ETraceType	GetTraceType() const;
	CBaseEntity* pSkip = nullptr;
};