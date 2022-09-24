#include "Util.h"
#include "Logger.h"
#include <windows.h>

bool GetCurrentModuleInfo(uint8_t*& codeBase, size_t& codeSectionSize)
{
	uint8_t* moduleBase = reinterpret_cast<uint8_t*>(GetModuleHandleA(NULL));
	codeBase = nullptr;
	codeSectionSize = 0;

	if (moduleBase == nullptr)
	{
		Log("moduleBase not found");
		return false;
	}

	Log("moduleBase %llx", reinterpret_cast<size_t>(moduleBase));

	for (const uint8_t* addr = moduleBase;;)
	{
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi));
		if (mbi.State & 0x10000)
		{
			break;
		}

		addr += mbi.RegionSize;

		if (codeBase == nullptr && mbi.BaseAddress != moduleBase)
		{
			codeBase = reinterpret_cast<uint8_t*>(mbi.BaseAddress);
			codeSectionSize = mbi.RegionSize;
			break;
		}
	}

	if (codeBase == nullptr)
	{
		Log("codeBase not found");
		return false;
	}

	return true;
}

uint8_t* SearchPattern(const uint16_t* pattern, size_t patternSize, uint8_t* ptr, size_t start, size_t size)
{
	for (size_t i = start; i < start + size; i++)
	{
		size_t j;
		for (j = 0; j < patternSize; j++)
			if (pattern[j] < 0x100 && ptr[i + j] != pattern[j])
				break;
		if (j == patternSize)
			return ptr + i;
	}

	return nullptr;
}
