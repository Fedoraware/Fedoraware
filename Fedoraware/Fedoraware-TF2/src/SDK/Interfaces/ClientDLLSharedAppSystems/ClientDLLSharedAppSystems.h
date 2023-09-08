#pragma once
#include "../../Includes/Includes.h"

class CClientDLLSharedAppSystems
{
public:
	virtual int	Count() = 0;
	virtual char const *GetDllName(int idx) = 0;
	virtual char const *GetInterfaceName(int idx) = 0;
};

#define CLIENT_DLL_SHARED_APPSYSTEMS "VClientDllSharedAppSystems001"