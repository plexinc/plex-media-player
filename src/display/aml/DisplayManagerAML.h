#ifndef DISPLAYMANAGERAML_H_
#define DISPLAYMANAGERAML_H_

#include <qmetatype.h>
#include "display/DisplayManager.h"
#include <QMap>

class DisplayManagerAML : public DisplayManager
{
  Q_OBJECT
private:
  QMap<int, QString> modeStrings;

public:
  DisplayManagerAML(QObject* parent) : DisplayManager(parent) {};

  virtual bool initialize();
  virtual bool setDisplayMode(int display, int mode);
  virtual int getCurrentDisplayMode(int display);
  virtual int getMainDisplay();
  virtual int getDisplayFromPoint(int x, int y);
};

#endif /* DISPLAYMANAGERAML_H_ */
