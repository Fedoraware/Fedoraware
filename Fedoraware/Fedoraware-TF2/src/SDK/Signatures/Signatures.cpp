#include "Signatures.h"
#include "../../Utils/Pattern/Pattern.h"
#include <format>

void CSignature::Find()
{
	m_Address = g_Pattern.Find(m_Module, m_Pattern);
	if (m_Address == 0)
	{
		const auto msg = std::format("Module: {}\nPattern: {}\nAddress: {:#x} (+{:d})", m_Module, m_Pattern, m_Address, m_Offset);
		MessageBoxA(nullptr, msg.c_str(), "Pattern not found", MB_OK | MB_ICONERROR);
	}

	m_Address += m_Offset;
}
