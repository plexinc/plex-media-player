#ifndef OESYSTEMCOMPONENT_H
#define OESYSTEMCOMPONENT_H

#include <QObject>
#include <QDateTime>

#include "ComponentManager.h"

#define TZDATA_ZONE_FILE "/usr/share/zoneinfo/zone.tab"

#define SETTINGS_KEY_TZMAIN   "timeZoneMain"
#define SETTINGS_KEY_TZREGION "timeZoneRegion"

class OESystemComponent : public ComponentBase
{
  Q_OBJECT
  DEFINE_SINGLETON(OESystemComponent)

private:
  QMap<QString, QVariantList> m_timezones;
  QString m_currentTzMain;
  QString m_currentTzRegion;

public:
  OESystemComponent(QObject *parent = 0);

  virtual bool componentExport() { return true; }
  virtual const char* componentName() { return "oesystem"; }
  virtual bool componentInitialize();

  Q_INVOKABLE qint64 getTime() { return QDateTime::currentDateTime().toMSecsSinceEpoch(); }

  bool setHostName(QString name);

  // timezone related functions
  bool loadTimeZones(QString filename);
  void setTimeZone(QString timezone);

public Q_SLOTS:
  void mainSettingsChanged();

 Q_SIGNALS:
  void timezoneChanged();
};

#endif // OESYSTEMCOMPONENT_H
