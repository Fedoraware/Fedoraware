#pragma once
#include <vector>
//class CHooks
//{
//private:
//	HWND m_hwWindow = 0;
//	void* m_pOriginal;
//
//public:
//	void Init();
//	void Release();
//	void Create(void* pSrc, void* pDst)
//	{
//		if (MH_CreateHook(pSrc, pDst, &m_pOriginal) != MH_STATUS::MH_OK)
//			return;
//	}
//
//	template <typename FN>
//	inline FN CallOriginal()
//	{
//		return reinterpret_cast<FN>(m_pOriginal);
//	}
//};

inline std::vector<void*>& GetVecHooks()
{
	static std::vector<void*> s_vecHooks;
	return s_vecHooks;
}

class CHookManager
{
public:
	void Init();
	void Release();
};

inline CHookManager g_HookManager;