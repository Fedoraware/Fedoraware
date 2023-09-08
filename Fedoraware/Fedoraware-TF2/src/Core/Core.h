#pragma once

class CCore
{
	void OnLoaded();

public:
	void Load();
	void Unload();

	bool ShouldUnload();
};

inline CCore g_Core;