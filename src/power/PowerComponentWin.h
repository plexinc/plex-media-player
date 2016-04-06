#ifndef POWERCOMPONENTWIN_H
#define POWERCOMPONENTWIN_H

#include "PowerComponent.h"
#include "utils/Utils.h"

class PowerComponentWin : public PowerComponent
{
public:
  PowerComponentWin() : PowerComponent(0) { }
  virtual void doDisableScreensaver();
  virtual void doEnableScreensaver();

  virtual bool canPowerOff() override { return WinUtils::getPowerManagementPrivileges(); }
  virtual bool canReboot() override { return WinUtils::getPowerManagementPrivileges(); }
  virtual bool canSuspend() override { return true; }

  virtual bool PowerOff() override;
  virtual bool Reboot() override;
  virtual bool Suspend() override;
};

#endif // POWERCOMPONENTWIN_H
