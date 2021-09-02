#pragma once
#include "MinHook.h"

class CHook
{
private:
	void *m_pOriginal;

public:
	void Create(void *pSrc, void *pDst)
	{
		if (MH_CreateHook(pSrc, pDst, &m_pOriginal) != MH_STATUS::MH_OK)
			return;
	}

	template <typename FN>
	inline FN CallOriginal()
	{
		return reinterpret_cast<FN>(m_pOriginal);
	}
};