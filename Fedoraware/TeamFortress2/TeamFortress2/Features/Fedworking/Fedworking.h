#pragma once
#include "../../SDK/SDK.h"

class CFedworking
{
private:
	void ConsoleLog(const std::string& pMessage);

public:
	void HandleMessage(const char* pMessage);
	void SendMarker(const Vec3& pPos, int pEntityIndex);
	void SendMessage(const std::string& pData);
	void Run();
};

inline CFedworking g_Fedworking;
