#include "settings/SettingsComponent.h"
#include "settings/SettingsSection.h"
#include "OESystemComponent.h"
#include "QsLog.h"
#include <unistd.h>
#include <QFile>
#include <QByteArray>
#include <QDateTime>

///////////////////////////////////////////////////////////////////////////////////////////////////
OESystemComponent::OESystemComponent(QObject *parent) : ComponentBase(parent)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OESystemComponent::componentInitialize()
{
  setHostName(SettingsComponent::Get().value(SETTINGS_SECTION_OPENELEC, "systemname").toString());

  if (!loadTimeZones(TZDATA_ZONE_FILE))
  {
    QLOG_ERROR() << "unable to load timezones.";
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OESystemComponent::setHostName(QString name)
{
  // first we change the hostname name for this session
  char* hostname = name.toUtf8().data();
  sethostname(hostname, strlen(hostname));

  // then edit the hostname file so that its persistent
  QFile file("/storage/.cache/hostname");
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QLOG_ERROR() << "setHostName : Failed to open" << file.fileName();
    return false;
  }

  QTextStream writer(&file);
  writer << name;
  file.close();

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OESystemComponent::loadTimeZones(QString filename)
{
  QFile zonefile(filename);

  if (!zonefile.open(QIODevice::ReadOnly))
  {
    QLOG_ERROR() << "Unable to open tzdata zone file " << filename;
    return false;
  }

  while (!zonefile.atEnd())
  {
    QString line = zonefile.readLine();
    if (!line.startsWith("#"))
    {
      QStringList fields = line.replace("\n", "").split("\t");
      if (fields.size() >= 3)
      {
        QStringList tzFields = fields.at(2).split("/");
        QVariantMap entry;
        entry["value"] = tzFields.at(1);
        entry["title"] = tzFields.at(1);
        m_timezones[tzFields.at(0)] << entry;
      }
    }
  }

  zonefile.close();

  // build the settings variant map
  QVariantList tzMainList;

  foreach(QString tzMain, m_timezones.keys())
  {
    QVariantMap entry;
    entry["value"] = tzMain;
    entry["title"] = tzMain;
    tzMainList << entry;
  }

  SettingsComponent::Get().updatePossibleValues(SETTINGS_SECTION_MAIN, "timeZoneMain", tzMainList);

  connect(SettingsComponent::Get().getSection(SETTINGS_SECTION_MAIN), &SettingsSection::valuesUpdated,
            this, &OESystemComponent::mainSettingsChanged);

  mainSettingsChanged();
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OESystemComponent::mainSettingsChanged()
{
  bool tzChanged = false;
  QString settingTzMain = SettingsComponent::Get().value(SETTINGS_SECTION_MAIN, SETTINGS_KEY_TZMAIN).toString();
  QString settingTzRegion = SettingsComponent::Get().value(SETTINGS_SECTION_MAIN, SETTINGS_KEY_TZREGION).toString();

  // Check if we have a change in the timezone
  if (settingTzMain != m_currentTzMain)
  {
    tzChanged = true;
    m_currentTzMain = settingTzMain;

    // check if current region is in that list
    bool isRegionValid = false;
    foreach(QVariant entry, m_timezones[m_currentTzMain])
    {
      if (entry.toMap()["value"].toString() == settingTzRegion)
      {
        isRegionValid = true;
        m_currentTzRegion = settingTzRegion;
        break;
       }
    }

    // if it's not valid, default to first entry
    if (!isRegionValid)
    {
      QVariantMap entry = m_timezones[m_currentTzMain].at(0).toMap();
      m_currentTzRegion = entry["value"].toString();
      settingTzRegion = m_currentTzRegion;
    }
  }

  if (settingTzRegion !=   m_currentTzRegion)
  {
    tzChanged = true;
    m_currentTzRegion = settingTzRegion;
  }

  if (tzChanged)
  {
    SettingsComponent::Get().setValue(SETTINGS_SECTION_MAIN, SETTINGS_KEY_TZREGION, m_currentTzRegion);

     // update the list of region according to main zone
    SettingsComponent::Get().updatePossibleValues(SETTINGS_SECTION_MAIN, SETTINGS_KEY_TZREGION,  m_timezones[m_currentTzMain]);

    setTimeZone(QString("%1/%2").arg(m_currentTzMain).arg(m_currentTzRegion));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OESystemComponent::setTimeZone(QString timezone)
{
  qputenv("TZ", timezone.toLocal8Bit());
  QLOG_DEBUG() << "Timezone set to" << timezone << ", time is now" << QDateTime::currentDateTime().toString();
  emit timezoneChanged();
}
