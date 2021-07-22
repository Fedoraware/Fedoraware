#include "KeyValues.h"

#pragma warning (disable : 6031)

bool CKeyValUtils::LoadFromBuffer(KeyValues *key_value, char const *resource_name, const char *buffer, void *file_system, const char *path_id)
{
	using FN = int(__thiscall *)(KeyValues *, char const *, const char *, void *, const char *);
	static FN load_from_the_buffer = (FN)g_Pattern.Find(_(L"engine.dll"), _(L"55 8B EC 83 EC 38 53 8B 5D 0C"));
	return load_from_the_buffer(key_value, resource_name, buffer, file_system, path_id);
}

KeyValues *CKeyValUtils::Initialize(KeyValues *key_value, char *name)
{
	using FN = KeyValues * (__thiscall *)(KeyValues *, char *);
	static FN initialize = (FN)(g_Pattern.Find(_(L"engine.dll"), _(L"FF 15 ? ? ? ? 83 C4 08 89 06 8B C6")) - 0x42);
	return initialize(key_value, name);
}