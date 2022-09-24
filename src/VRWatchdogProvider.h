#pragma once
#include <openvr_driver.h>

class VRWatchdogProvider : public vr::IVRWatchdogProvider
{
	virtual vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override;
	virtual void Cleanup() override;
};
