#pragma once
#include "../MatRenderContext/MatRenderContext.h"
#include "../Texture/Texture.h"

typedef unsigned short MaterialHandle_t;

class CMaterialSystem
{
public:
	IMaterial* Find(char const* szMat, const char* szTextureGroup, bool bComplain = true, const char* szPrefix = NULL)
	{
		typedef IMaterial* (__thiscall* FN)(PVOID, const char*, const char*, bool, const char*);
		return GetVFunc<FN>(this, 73)(this, szMat, szTextureGroup, bComplain, szPrefix);
	}

	IMaterial* Create(char const* szName, void * pKV)
	{
		typedef IMaterial* (__thiscall* FN)(PVOID, const char*, void *);
		return GetVFunc<FN>(this, 72)(this, szName, pKV);
	}

	IMaterial* Get(MaterialHandle_t hMat)
	{
		typedef IMaterial* (__thiscall* FN)(PVOID, MaterialHandle_t);
		return GetVFunc<FN>(this, 78)(this, hMat);
	}

	MaterialHandle_t First()
	{
		typedef MaterialHandle_t(__thiscall* FN)(PVOID);
		return GetVFunc<FN>(this, 75)(this);
	}

	MaterialHandle_t Invalid()
	{
		typedef MaterialHandle_t(__thiscall* FN)(PVOID);
		return GetVFunc<FN>(this, 77)(this);
	}

	MaterialHandle_t Next(MaterialHandle_t hMat)
	{
		typedef MaterialHandle_t(__thiscall* FN)(PVOID, MaterialHandle_t);
		return GetVFunc<FN>(this, 76)(this, hMat);
	}

	ITexture *CreateNamedRenderTargetTextureEx(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED,
		unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		unsigned int renderTargetFlags = 0)
	{
		typedef ITexture *(__thiscall *FN)(PVOID, const char *, int, int, RenderTargetSizeMode_t, ImageFormat, MaterialRenderTargetDepth_t, unsigned int, unsigned int);
		return GetVFunc<FN>(this, 87)(this, pRTName, w, h, sizeMode, format, depth, textureFlags, renderTargetFlags);
	}

	ITexture* CreateFullFrameRenderTarget(const char* pName)
	{
		return CreateNamedRenderTargetTextureEx(
			pName, 1, 1, RT_SIZE_FULL_FRAME_BUFFER, IMAGE_FORMAT_RGB888,
			MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
			CREATERENDERTARGETFLAGS_HDR
		);
	}

	ITexture *FindTexture(char const *pTextureName, const char *pTextureGroupName, bool complain = true, int nAdditionalCreationFlags = 0)
	{
		typedef ITexture *(__thiscall *FN)(PVOID, char const *, const char *, bool, int);
		return GetVFunc<FN>(this, 81)(this, pTextureName, pTextureGroupName, complain, nAdditionalCreationFlags);
	}

	IMatRenderContext *GetRenderContext()
	{
		typedef IMatRenderContext *(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 100)(this);
	}
};

#define VMATERIALSYSTEM_INTERFACE "VMaterialSystem081"