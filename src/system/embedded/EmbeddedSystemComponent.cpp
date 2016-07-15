#include "settings/SettingsComponent.h"
#include "EmbeddedSystemComponent.h"
#include "QsLog.h"
#include <unistd.h>
#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////
EmbeddedSystemComponent::EmbeddedSystemComponent(QObject *parent) : ComponentBase(parent)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EmbeddedSystemComponent::componentInitialize()
{
  setHostName(SettingsComponent::Get().value(SETTINGS_SECTION_EMBEDDED, "systemname").toString());

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EmbeddedSystemComponent::setHostName(QString name)
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
