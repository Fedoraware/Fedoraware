#pragma once
#include "../BaseEntity/BaseEntity.h"

class CKeyValUtils
{
public:
	bool LoadFromBuffer(KeyValues *key_value, char const *resource_name, const char *buffer, void *file_system = 0, const char *path_id = 0);
	KeyValues *Initialize(KeyValues *key_value, char *name);
};

inline CKeyValUtils g_KeyValUtils;