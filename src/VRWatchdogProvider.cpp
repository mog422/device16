#include "VRWatchdogProvider.h"


vr::EVRInitError VRWatchdogProvider::Init(vr::IVRDriverContext* pDriverContext)
{
	VR_INIT_WATCHDOG_DRIVER_CONTEXT(pDriverContext);
	return vr::VRInitError_None;
}

void VRWatchdogProvider::Cleanup()
{
	VR_CLEANUP_WATCHDOG_DRIVER_CONTEXT()
}
