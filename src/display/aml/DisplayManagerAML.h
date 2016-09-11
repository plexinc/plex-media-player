#ifndef DISPLAYMANAGERAML_H_
#define DISPLAYMANAGERAML_H_

#include <qmetatype.h>
#include "display/DisplayManager.h"
#include <QMap>
#include <QSize>
#include <linux/fb.h>

#define AML_FRAMBUFFER_WIDTH    1920
#define AML_FRAMBUFFER_HEIGHT   1080

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
  bool setFramebufferResolution(QSize fbsize);

private :
  bool getFramebufferResolution(fb_var_screeninfo *vinfo);
  bool setFramebufferResolution(fb_var_screeninfo *vinfo);
  bool setupScaling();
};

#endif /* DISPLAYMANAGERAML_H_ */
