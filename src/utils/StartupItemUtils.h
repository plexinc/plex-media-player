//
// Created by Tobias Hieta on 15/08/16.
//

#ifndef PLEXMEDIAPLAYER_STARTUPITEMUTILS_H
#define PLEXMEDIAPLAYER_STARTUPITEMUTILS_H

#include <QObject>

class StartupItemUtils : public QObject
{
  Q_OBJECT
public:
  StartupItemUtils();
  virtual void addStartupItem() = 0;
  virtual void removeStartupItem() = 0;

private:
  Q_SLOT void mainSettingsUpdated(const QVariantMap& map);
  void updateStartupItem();
};

#endif //PLEXMEDIAPLAYER_STARTUPITEMUTILS_H
