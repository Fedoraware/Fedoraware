#pragma once
#include "../../SDK/SDK.h"

class CFedworking
{
public:
	void HandleMessage(const char* pMessage);
	void SendMarker(const Vec3& pPos, const std::string& pTitle);
	void SendMessage(const std::string& pData);
};

inline CFedworking g_Fedworking;
