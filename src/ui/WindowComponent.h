#ifndef WINDOWCOMPONENT_H
#define WINDOWCOMPONENT_H

#include <QsLog.h>
#include "ComponentManager.h"
#include "KonvergoWindow.h"

class WindowComponent : public ComponentBase
{
  Q_OBJECT
public:

  static WindowComponent& Get();

  explicit WindowComponent(QObject* parent = nullptr)
  : ComponentBase(parent)
    { }

  bool componentInitialize() override;
  bool componentExport() override { return true; }
  const char* componentName() override { return "window"; }

  void setWindow(KonvergoWindow *window);

public Q_SLOTS:
  void onVisibilityChanged(QWindow::Visibility visibility);

Q_SIGNALS:
  void visibilityChanged(QWindow::Visibility visibility);

};


#endif // WINDOWCOMPONENT_H

