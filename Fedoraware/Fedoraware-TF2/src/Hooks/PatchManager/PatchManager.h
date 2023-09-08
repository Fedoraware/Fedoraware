#pragma once
#include <Windows.h>
#include <vector>

class BytePatch;

class CPatchManager
{
public:
	void Init();
	void Restore();

	std::vector<BytePatch*>& GetVecPatches()
	{
		static std::vector<BytePatch*> vecPatches;
		return vecPatches;
	}
};

inline CPatchManager g_PatchManager;

class BytePatch
{
	LPVOID m_Address = nullptr;
	std::vector<BYTE> m_Original;
	std::vector<BYTE> m_Patch;
	SIZE_T m_Size = 0;
	bool m_IsPatched = false;

	void Write(PVOID address, const std::vector<BYTE>& bytes, size_t length)
	{
		DWORD flNewProtect, flOldProtect;
		VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &flNewProtect);
		memcpy(address, bytes.data(), length);
		VirtualProtect(address, length, flNewProtect, &flOldProtect);
	}

public:
	BytePatch(DWORD address, const std::vector<BYTE>& patch)
	{
		m_Address = reinterpret_cast<LPVOID>(address);
		m_Patch = patch;
		m_Size = m_Patch.size();
		m_Original.resize(m_Size);

		// Copy the original bytes
		DWORD flNewProtect, flOldProtect;
		VirtualProtect(m_Address, m_Size, PAGE_EXECUTE_READWRITE, &flNewProtect);
		memcpy(m_Original.data(), m_Address, m_Size);
		VirtualProtect(m_Address, m_Size, flNewProtect, &flOldProtect);

		g_PatchManager.GetVecPatches().emplace_back(this);
	}

	void Patch()
	{
		if (m_IsPatched) { return; }

		Write(m_Address, m_Patch, m_Size);
		m_IsPatched = true;
	}

	void Restore()
	{
		if (!m_IsPatched) { return; }

		Write(m_Address, m_Original, m_Size);
		m_IsPatched = false;
	}
};
