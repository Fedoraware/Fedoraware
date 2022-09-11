#pragma once
#include "../../Includes/Includes.h"
#include "RenderView/RenderView.h"

class IOcclusionSystem
{
public:
	virtual void ActivateOccluder( int nOccluderIndex, bool bActive ) = 0;
	virtual void SetView( const Vec3 &vecCameraPos, float flFOV, const VMatrix &worldToCamera, const VMatrix &cameraToProjection, const VPlane &nearClipPlane ) = 0;
	virtual bool IsOccluded( const Vec3 &vecAbsMins, const Vec3 &vecAbsMaxs ) = 0;
	virtual void SetOcclusionParameters( float flMaxOccludeeArea, float flMinOccluderArea ) = 0;
	virtual float MinOccluderArea() const = 0;
	virtual void DrawDebugOverlays() = 0;
};

