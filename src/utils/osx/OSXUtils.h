#ifndef OSXUTILS_H
#define OSXUTILS_H

#include <QString>
#include <ApplicationServices/ApplicationServices.h>

class KonvergoWindow;

namespace OSXUtils
{
  QString ComputerName();
  OSStatus SendAppleEventToSystemProcess(AEEventID eventToSendID);

  void SetCursorVisible(bool visible);

  void SetWindowRestoration(KonvergoWindow* window);
};

#endif /* OSXUTILS_H */
