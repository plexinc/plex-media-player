#ifndef OSXUTILS_H
#define OSXUTILS_H

#include <QString>
#include <ApplicationServices/ApplicationServices.h>

namespace OSXUtils
{
  void SetMenuBarVisible(bool visible);
  QString ComputerName();
  OSStatus SendAppleEventToSystemProcess(AEEventID eventToSendID);
  void HideMainWindow();
  void UnhideMainWindow();
  bool IsMainWindowHidden();
};

#endif /* OSXUTILS_H */
