#pragma once
#include "../../SDK/SDK.h"

class CFedworking
{
private:
	Vec3 ReadPos(std::string pData, int pStart);

public:
	void HandleMessage(const char* pMessage);
};

inline CFedworking g_Fedworking;
