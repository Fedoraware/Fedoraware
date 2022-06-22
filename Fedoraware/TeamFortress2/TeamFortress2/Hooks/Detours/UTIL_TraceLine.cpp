#include "../Hooks.h"

MAKE_HOOK(UTIL_TraceLine, g_Pattern.Find(L"client.dll", L"53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C ? ? 8B EC 83 EC ? 8D 4D ? 56 FF 73 ? FF 73 ? E8 ? ? ? ? 6A"), void, __cdecl,
    void* ecx, void* edx, Vector* vecAbsStart, Vector* vecAbsEnd, unsigned int mask, CTraceFilter* pFilter, CGameTrace* ptr)
{
    static auto dwDisplayDamageFeedback = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? F3 0F 10 45 ? 83 C4 18 0F 2F 05 ? ? ? ? 8B 75 10") + 0x5;

    if (reinterpret_cast<DWORD>(_ReturnAddress()) == dwDisplayDamageFeedback && pFilter) {
        *reinterpret_cast<float*>(reinterpret_cast<DWORD>(pFilter) + 0x2C) = 1.0f;
        return;
    }

    return Hook.Original<FN>()(ecx, edx, vecAbsStart, vecAbsEnd, mask, pFilter, ptr);
}