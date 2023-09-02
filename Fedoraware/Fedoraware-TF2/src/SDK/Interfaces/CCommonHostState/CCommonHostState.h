#pragma once
#include "../../Includes/Includes.h"

class mtexinfo_t
{
	char pad[76];
public:
	IMaterial* material;
};

class worldbrushdata_t
{
	char pad[88];
public:
	int numtexinfo;
	mtexinfo_t* texinfo;

	int         numtexdata;
	csurface_t* texdata;
};

struct brushdata_t
{
	worldbrushdata_t* pShared;
	int			firstmodelsurface, nummodelsurfaces;

	unsigned short	renderHandle;
	unsigned short	firstnode;
};

struct spritedata_t
{
	int				numframes;
	int				width;
	int				height;
	CEngineSprite* sprite;
};

typedef unsigned short MDLHandle_t;

class CCommonHostState
{
public:
	model_t* worldmodel;	// cl_entitites[0].model
	worldbrushdata_t* worldbrush;
	float		interval_per_tick;		// Tick interval for game
};

