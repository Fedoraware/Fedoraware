#pragma once
#include "../Hooks.h"
namespace Viewmodel
{
	inline SEOHook::Func Func;
	void Init();
	using fn = void(__fastcall*)(void*, void*, CBaseEntity*, Vec3&, Vec3&);
	void __fastcall Hook(void * ecx, void *edx, CBaseEntity* owner, Vec3 &eyePosition, Vec3 &eyeAngles);
}