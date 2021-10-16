#pragma once

#include "../../Includes/Includes.h"

class CModelInfoClient
{
public:
	model_t *GetModel(int index)
	{
		typedef model_t *(__thiscall *FN)(PVOID, int);
		return GetVFunc<FN>(this, 1)(this, index);
	}

	int	GetModelIndex(const char* name)
	{
		typedef int (__thiscall *FN)(PVOID, const char *);
		return GetVFunc<FN>(this, 2)(this, name);
	}

	const char *GetModelName(const model_t *model) 
	{
		typedef const char *(__thiscall *FN)(PVOID, const model_t *);
		return GetVFunc<FN>(this, 3)(this, model);
	}

	studiohdr_t *GetStudioModel(const model_t *model)
	{
		typedef studiohdr_t *(__thiscall *FN)(PVOID, const model_t *);
		return GetVFunc<FN>(this, 28)(this, model);
	}
};

#define VMODELINFO_CLIENT_INTERFACE_VERSION	"VModelInfoClient006"