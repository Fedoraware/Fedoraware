#pragma once
#include <map>
#include <string>
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

class CHook;

class CHookManager
{
public:
	void Init();
	void Release();

	std::map<std::string, CHook*>& GetMapHooks()
	{
		static std::map<std::string, CHook*> mapHooks;
		return mapHooks;
	}
};

inline CHookManager g_HookManager;