#include "ServerTrackedDeviceProvider.h"
#include "Logger.h"
#include "Util.h"
#include <cstdint>
#include <cassert>
#include <windows.h>

static bool StartHook();

vr::EVRInitError ServerTrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
	Log("ServerTrackedDeviceProvider::Init");

	if (StartHook() == false)
	{
		Log("Failed to Hook");
	}

	return vr::VRInitError_None;
}

void ServerTrackedDeviceProvider::Cleanup()
{
	VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

static uint64_t AssignDeviceIndex(uint64_t start, uint64_t end, char* baseTable, const char* serial)
{
	int i = 0;
	Log("AssignDeviceIndex start:%llu end:%llu serial:%s", start, end, serial);

	if (strncmp(serial, "LHR-", 4))
	{
		start = 16;
		end = vr::k_unMaxTrackedDeviceCount;
	}

	for (i = static_cast<int>(start); i < static_cast<int>(end); i++)
	{
		if (*reinterpret_cast<uint64_t*>(baseTable + (48 * i) + 8))
		{
			continue;
		}

		Log("AssignDeviceIndex assigned %d (in loop)", i);
		return i;
	}

	Log("AssignDeviceIndex assigned %d (last)", i);
	return i;
}

static bool StartHook()
{
	static const uint16_t targetPattern[] = {
		0x73, 0x26, 0x0F, 0x1F, 0x40, 0x00, 0x66, 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x48, 0x8D, 0x04, 0x40, 0x48, 0x03, 0xC0, 0x49, 0x83, 0x7C, 0xC0, 0x08, 0x00, 0x74, 0x09, 0xFF,
		0xC6, 0x8B, 0xC6, 0x48, 0x3B, 0xC2, 0x72, 0xE8, 0x83, 0xFE, 0x40, 0x72, 0x45, 0xB8, 0x78, 0x00,
		0x00, 0x00, 0xE9, 0xF1, 0x05, 0x00, 0x00
		//.text:000000014012FE55                                     loc_14012FE55:
		//.text:000000014012FE55 B9 01 00 00 00                                      mov     ecx, 1
		//.text:000000014012FE5A 48 89 B4 24 B0 00 00 00                             mov     [rsp+0A0h+arg_0], rsi
		//.text:000000014012FE62 44 3B D1                                            cmp     r10d, ecx
		//.text:000000014012FE65 74 70                                               jz      short loc_14012FED7
		//.text:000000014012FE67 42 80 7C 00 44 00                                   cmp     byte ptr [rax+r8+44h], 0
		//.text:000000014012FE6D BE 10 00 00 00                                      mov     esi, 10h
		//.text:000000014012FE72 4D 8B 46 20                                         mov     r8, [r14+20h]
		//.text:000000014012FE76 48 B8 AB AA AA AA AA AA AA 2A                       mov     rax, 2AAAAAAAAAAAAAABh
		//.text:000000014012FE80 0F 44 F1                                            cmovz   esi, ecx
		//.text:000000014012FE83 49 8B 4E 28                                         mov     rcx, [r14+28h]
		//.text:000000014012FE87 49 2B C8                                            sub     rcx, r8
		//.text:000000014012FE8A 48 F7 E9                                            imul    rcx
		//.text:000000014012FE8D 48 C1 FA 03                                         sar     rdx, 3
		//.text:000000014012FE91 48 8B C2                                            mov     rax, rdx
		//.text:000000014012FE94 48 C1 E8 3F                                         shr     rax, 3Fh
		//.text:000000014012FE98 48 03 D0                                            add     rdx, rax
		//.text:000000014012FE9B 8B C6                                               mov     eax, esi
		//.text:000000014012FE9D 48 3B C2                                            cmp     rax, rdx
		//.text:000000014012FEA0 73 26                                               jnb     short loc_14012FEC8
		//.text:000000014012FEA2 0F 1F 40 00                                         nop     dword ptr [rax+00h]
		//.text:000000014012FEA6                                                     db      66h, 66h
		//.text:000000014012FEA6 66 66 0F 1F 84 00 00 00 00 00                       nop     word ptr [rax+rax+00000000h]
		//.text:000000014012FEB0
		//.text:000000014012FEB0                                     assignDeviceIDLoop:
		//.text:000000014012FEB0 48 8D 04 40                                         lea     rax, [rax+rax*2]
		//.text:000000014012FEB4 48 03 C0                                            add     rax, rax
		//.text:000000014012FEB7 49 83 7C C0 08 00                                   cmp     qword ptr [r8+rax*8+8], 0
		//.text:000000014012FEBD 74 09                                               jz      short loc_14012FEC8
		//.text:000000014012FEBF FF C6                                               inc     esi
		//.text:000000014012FEC1 8B C6                                               mov     eax, esi
		//.text:000000014012FEC3 48 3B C2                                            cmp     rax, rdx
		//.text:000000014012FEC6 72 E8                                               jb      short assignDeviceIDLoop
		//.text:000000014012FEC8
		//.text:000000014012FEC8                                     loc_14012FEC8:
		//.text:000000014012FEC8 83 FE 40                                            cmp     esi, 40h
		//.text:000000014012FECB 72 45                                               jb      short loc_14012FF12
		//.text:000000014012FECD B8 78 00 00 00                                      mov     eax, 78h
		//.text:000000014012FED2 E9 F1 05 00 00                                      jmp     loc_1401304C8

	};

	uint8_t hookCode[] = {
			0x51,                       // push   rcx
			0x52,                       // push   rdx
			0x41, 0x50,                 // push   r8
			0x41, 0x51,                 // push   r9
			0x41, 0x52,                 // push   r10
			0x41, 0x53,                 // push   r11

			0x48, 0x89, 0xf1,           // mov    rcx,rsi  (start index)
										// keep rdx (end index)
										// keep r8 (base table)
			0x4c, 0x8b, 0x4d, 0x77,     // mov    r9,QWORD PTR [rbp+0x77]   serial

			0x48, 0xb8, 0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde,
										// movabs rax,0xdeadbeefdeadbeef
			0xff, 0xd0,                 // call   rax

			0x41, 0x5b,                 // pop    r11
			0x41, 0x5a,                 // pop    r10
			0x41, 0x59,                 // pop    r9
			0x41, 0x58,                 // pop    r8
			0x5a,                       // pop    rdx
			0x59,                       // pop    rcx
			0x48, 0x89, 0xc6,           // mov    rsi,rax
			0x48, 0xb8, 0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde,
										// movabs rax,0xdeadbeefdeadbeef
			0xff, 0xe0,                 // jmp    rax
	};

	uint8_t* codeBase;
	size_t codeSectionSize;
	if (GetCurrentModuleInfo(codeBase, codeSectionSize) == false)
	{
		return false;
	}

	Log("codeBase %llx-%llx", reinterpret_cast<size_t>(codeBase), reinterpret_cast<size_t>(codeBase + codeSectionSize));

	void* targetAddr = reinterpret_cast<void*>(SearchPattern(targetPattern, sizeof(targetPattern) / sizeof(*targetPattern), codeBase, 0, codeSectionSize));
	if (targetAddr == nullptr)
	{
		Log("searchPattern failed");
		return false;
	}

	Log("targetAddr:%llx", reinterpret_cast<size_t>(targetAddr));

	const size_t pageSize = 4096;
	static_assert(sizeof(hookCode) < pageSize, "hook code too big");

	void* hookCodeAddr = VirtualAlloc(nullptr, pageSize, MEM_COMMIT, PAGE_READWRITE);

	if (hookCodeAddr == nullptr)
	{
		Log("hookCodeAddr is null");
		return false;
	}

	Log("hookCodeAddr:%llx", reinterpret_cast<size_t>(hookCodeAddr));

	*reinterpret_cast<uint64_t*>(&hookCode[19]) = reinterpret_cast<uint64_t>(AssignDeviceIndex);
	*reinterpret_cast<uint64_t*>(&hookCode[44]) = reinterpret_cast<uint64_t>(targetAddr) + 40;

	memcpy(hookCodeAddr, hookCode, sizeof(hookCode));

	DWORD oldProt;
	if (VirtualProtect(hookCodeAddr, pageSize, PAGE_EXECUTE_READ, &oldProt) == FALSE)
	{
		Log("VirtualProtect1 failed");
		return false;
	}

	unsigned char targetHookCode[] = {
		0x48, 0xb8, 0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde,
					// movabs rax,0xdeadbeefdeadbeef
		0xff, 0xe0, // jmp    rax
	};

	*reinterpret_cast<uint64_t*>(&targetHookCode[2]) = reinterpret_cast<uint64_t>(hookCodeAddr);

	if (VirtualProtect(reinterpret_cast<LPVOID>(targetAddr), sizeof(targetHookCode), PAGE_READWRITE, &oldProt) == FALSE)
	{
		Log("VirtualProtect2 failed");
		return false;
	}

	memcpy(targetAddr, targetHookCode, sizeof(targetHookCode));

	if (VirtualProtect(targetAddr, sizeof(targetHookCode), oldProt, &oldProt) == FALSE)
	{
		Log("VirtualProtect3 failed");
		return false;
	}

	return true;
}
