#pragma once
#include <vector>
#include <Windows.h>

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
	BytePatch(PVOID address, std::vector<unsigned char> patch)
	{
		PatchAddress = address;
		PatchBytes = patch;
		PatchLength = patch.size();
		OriginalBytes.resize(patch.size());

		// Copy the original bytes
		DWORD d, ds;
		VirtualProtect(address, PatchLength, PAGE_EXECUTE_READWRITE, &d);
		memcpy(&OriginalBytes[0], address, PatchLength);
		VirtualProtect(address, PatchLength, d, &ds);
	}

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