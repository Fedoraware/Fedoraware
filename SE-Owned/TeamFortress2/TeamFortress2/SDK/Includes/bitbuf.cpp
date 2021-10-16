#include "bitbuf.h"
#include "../../Utils/Utils.h"

int bf_write::StartWriting(void *pData, int nBytes, int iStartBit, int nBits)
{
	using fn = int(__thiscall *)(bf_write *, void *, int, int, int);
	static fn FN = reinterpret_cast<fn>(g_Pattern.Find(XorStr(L"engine.dll").c_str(), XorStr(L"55 8B EC 8B 45 08 8B 55 0C 83 E2 FC 89 01 8B 45 14").c_str()));
	return FN(this, pData, nBytes, iStartBit, nBits);
}