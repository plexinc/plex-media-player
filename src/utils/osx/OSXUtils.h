#ifndef OSXUTILS_H
#define OSXUTILS_H

#include <QString>
#include <ui/KonvergoWindow.h>

namespace OSXUtils
{
  void SetMenuBarVisible(bool visible);
  QString ComputerName();
  void HideMainWindow();
  void ShowMainWindow();
  bool IsWindowHidden();
};

#endif /* OSXUTILS_H */
