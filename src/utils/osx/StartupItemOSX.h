//
// Created by Tobias Hieta on 15/08/16.
//

#ifndef PLEXMEDIAPLAYER_STARTUPITEMOSX_H
#define PLEXMEDIAPLAYER_STARTUPITEMOSX_H

#include "utils/StartupItemUtils.h"

class StartupItemOSX : public StartupItemUtils
{
public:
  void addStartupItem() override;
  void removeStartupItem() override;
};

#endif //PLEXMEDIAPLAYER_STARTUPITEMOSX_H
