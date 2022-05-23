#pragma once

#include <vector>

class BytePatch;

class CPatchManager {
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