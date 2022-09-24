#include <windows.h>

#include "Logger.h"
#include "ServerTrackedDeviceProvider.h"
#include "VRWatchdogProvider.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		InitLogger();
		Log("device16 dll loaded");
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
	static ServerTrackedDeviceProvider server;
	static VRWatchdogProvider watchdog;

	if (!strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName))
	{
		return &server;
	}
	else if (!strcmp(vr::IVRWatchdogProvider_Version, pInterfaceName))
	{
		return &watchdog;
	}

	if (pReturnCode)
	{
		*pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
	}
	return nullptr;
}
