#pragma once
#include <openvr_driver.h>

class ServerTrackedDeviceProvider : public vr::IServerTrackedDeviceProvider
{
public:
	virtual vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext) override;
	virtual void Cleanup() override;
	virtual const char * const *GetInterfaceVersions() { return vr::k_InterfaceVersions; }
	virtual void RunFrame() { }
	virtual bool ShouldBlockStandbyMode() { return false; }
	virtual void EnterStandby() { }
	virtual void LeaveStandby() { }
};
