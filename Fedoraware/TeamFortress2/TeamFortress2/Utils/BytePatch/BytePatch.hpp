#pragma once
#include <vector>
#include <Windows.h>

// Use the PatchManager to create and manage your patches.
// Remember to restore all patches before unloading!

class BytePatch {
	PVOID PatchAddress;
	std::vector<unsigned char> OriginalBytes;
	std::vector<unsigned char> PatchBytes;
	size_t PatchLength;
	bool IsPatched = false;

	void Write(PVOID address, std::vector<unsigned char> bytes, size_t length)
	{
		DWORD d, ds;
		VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &d);
		memcpy(address, bytes.data(), length);
		VirtualProtect(address, length, d, &ds);
	}

public:
	BytePatch(DWORD address, const std::vector<unsigned char>& patch)
	{
		//assert(address != 0);

		PatchAddress = reinterpret_cast<PVOID>(address);
		PatchBytes = patch;
		PatchLength = patch.size();
		OriginalBytes.resize(patch.size());

		// Copy the original bytes
		DWORD d, ds;
		VirtualProtect(PatchAddress, PatchLength, PAGE_EXECUTE_READWRITE, &d);
		memcpy(&OriginalBytes[0], PatchAddress, PatchLength);
		VirtualProtect(PatchAddress, PatchLength, d, &ds);

		Patch();
	}

	BytePatch(LPCWSTR szModuleName, LPCWSTR szPattern, const std::vector<unsigned char>& patch) : BytePatch(g_Pattern.Find(szModuleName, szPattern), patch) { }

	void Patch()
	{
		if (!IsPatched)
		{
			Write(PatchAddress, PatchBytes, PatchLength);
			IsPatched = true;
		}
	}

	void Restore()
	{
		if (IsPatched)
		{
			Write(PatchAddress, OriginalBytes, PatchLength);
			IsPatched = false;
		}
	}
};