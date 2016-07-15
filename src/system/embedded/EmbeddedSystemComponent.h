#ifndef EMBEDDEDSYSTEMCOMPONENT_H
#define EMBEDDEDSYSTEMCOMPONENT_H

#include <QObject>

#include "ComponentManager.h"

class EmbeddedSystemComponent : public ComponentBase
{
  Q_OBJECT
  DEFINE_SINGLETON(EmbeddedSystemComponent)

public:
  EmbeddedSystemComponent(QObject *parent = 0);

  virtual bool componentExport() { return true; }
  virtual const char* componentName() { return "embeddedsystem"; }
  virtual bool componentInitialize();

  bool setHostName(QString name);
};

#endif // EMBEDDEDSYSTEMCOMPONENT_H
