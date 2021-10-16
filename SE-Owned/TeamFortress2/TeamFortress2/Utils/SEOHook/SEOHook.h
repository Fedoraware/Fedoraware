#pragma once

#include <memory>

#include "../MinHook/MinHook.h"

#pragma warning (push)
#pragma warning (disable : 26812) //unscoped enum

namespace SEOHook
{
	//PURPOSE: Hooking VFuncs via a Detour
	class VTable
	{
	private:
		uintptr_t** m_pBaseClass = nullptr;
		std::unique_ptr<uintptr_t[]> m_pOriginals = {};
		int m_nTableSize = 0;

	public:
		void Init(void* pBase)
		{
			m_pBaseClass = reinterpret_cast<uintptr_t **>(pBase);

			if (m_nTableSize == 0)
			{
				while (reinterpret_cast<uintptr_t*>(*m_pBaseClass)[m_nTableSize])
					m_nTableSize++;
			}

			m_pOriginals = std::make_unique<uintptr_t[]>(m_nTableSize);
		};

		void Hook(const int nIndex, void* pFunction)
		{
			void* pTramp = nullptr;

			//TODO: Some sort of debug output at least in _DEBUG_ mode.
			if (MH_CreateHook((*reinterpret_cast<void***>(m_pBaseClass))[nIndex], pFunction, &pTramp) != MH_STATUS::MH_OK)
				return;

			m_pOriginals[nIndex] = reinterpret_cast<uintptr_t>(pTramp);
		}

		void RemoveHook()
		{
			if (m_pBaseClass)
			{
				MH_DisableHook(*reinterpret_cast<void***>(m_pBaseClass));
			}
		}

		template <typename FN>
		inline FN Original(const int nIndex)
		{
			return reinterpret_cast<FN>(m_pOriginals[nIndex]);
		}

		int GetTableSize()
		{
			return m_nTableSize;
		}
	};

	//PURPOSE: Hooking Standalone Funcs via a Detour
	class Func
	{
	private:
		void *m_pOriginal = nullptr;

	public:
		void Hook(void *pSrc, void *pDst)
		{
			if (MH_CreateHook(pSrc, pDst, &m_pOriginal) != MH_STATUS::MH_OK)
				return;
		}

		void RemoveHook()
		{
			MH_DisableHook(m_pOriginal);
		}

		template <typename FN>
		inline FN Original()
		{
			return reinterpret_cast<FN>(m_pOriginal);
		}
	};
}

#pragma warning (pop)