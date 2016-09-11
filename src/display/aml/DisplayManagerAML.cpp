#include "DisplayManagerAML.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QThread>
#include "utils/aml/AMLUtils.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "QsLog.h"

#define FRAMEBUFFER_DEVICE    "/dev/fb0"

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DisplayManagerAML::initialize()
{
  // add the main display (we can only have one)
  DMDisplayPtr display = DMDisplayPtr(new DMDisplay);
  display->m_id = 0;
  display->m_name = "Main Display";

  m_displays[display->m_id] = display;

  // then look for available videomodes
  QFile file("/sys/devices/virtual/amhdmitx/amhdmitx0/disp_cap");

  if(!file.open(QIODevice::ReadOnly))
  {
    QLOG_ERROR() << "Failed to retrieve videomode list";
    return false;
  }

  QTextStream reader(&file);
  QRegularExpression regex("([0-9]{3,4})([pi])([0-9]{2,3})hz");
  QString line;
  int modeid = 0;

  QStringList modeList;

  while (reader.readLineInto(&line))
  {
    modeList << line;
  }

  foreach (line, modeList)
  {
    QRegularExpressionMatch match = regex.match(line);
    if (match.hasMatch())
    {
      // add the videomode to the display
      DMVideoModePtr mode = DMVideoModePtr(new DMVideoMode);
      mode->m_id = modeid;
      display->m_videoModes[modeid] = mode;

      switch(match.captured(1).toInt())
      {
        case 480:
           mode->m_width = 720;
           mode->m_height = 480;
        break;

        case 576:
           mode->m_width = 720;
           mode->m_height = 576;
        break;

        case 720:
           mode->m_width = 1280;
           mode->m_height = 720;
        break;

        case 1080:
           mode->m_width = 1920;
           mode->m_height = 1080;
        break;

        case 2160:
           mode->m_width = 3840;
           mode->m_height = 2160;
        break;

        default:
          QLOG_WARN() << "Unknown video mode" << match.captured(1).toInt() << "p ?";
        break;
      }

      mode->m_refreshRate = match.captured(3).toInt();
      mode->m_bitsPerPixel = 32;
      mode->m_interlaced = match.captured(2) == "i" ? true : false;

      // store the mdoe string without asterisk (current video mode)
      modeStrings[modeid] = line.replace("*","");

      modeid++;
    }
  }

  file.close();

  // setup scaling if needed. when framebuffer is limited to 1080p
  // and that we run a 4k mode, this is necessary
  if (!setupScaling())
  {
    QLOG_ERROR() << "Failed to setup scaling";
  }

  return DisplayManager::initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DisplayManagerAML::getFramebufferResolution(fb_var_screeninfo *vinfo)
{
  int fd = open(FRAMEBUFFER_DEVICE, O_RDWR);
  int ret = false;

  if (fd > 0)
  {
   if (ioctl(fd, FBIOGET_VSCREENINFO, vinfo) == 0)
     ret = true;
   else
     QLOG_ERROR() << "Failed to get framebuffer resolution for " << FRAMEBUFFER_DEVICE;

   close(fd);
  }
  else
  {
    QLOG_ERROR() << "Failed to open framebuffer device " << FRAMEBUFFER_DEVICE;
  }

  return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DisplayManagerAML::setFramebufferResolution(fb_var_screeninfo *vinfo)
{
  int fd = open(FRAMEBUFFER_DEVICE, O_RDWR);
  int ret = false;

  if (fd > 0)
  {
   if (ioctl(fd, FBIOPUT_VSCREENINFO, vinfo) == 0)
     ret = true;
   else
     QLOG_ERROR() << "Failed to set framebuffer resolution for " << FRAMEBUFFER_DEVICE;

   close(fd);
  }
  else
  {
    QLOG_ERROR() << "Failed to open framebuffer device " << FRAMEBUFFER_DEVICE;
  }

  return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DisplayManagerAML::setFramebufferResolution(QSize fbsize)
{
  fb_var_screeninfo vinfo;

  if (getFramebufferResolution(&vinfo))
  {
    vinfo.xres = fbsize.width();
    vinfo.yres = fbsize.height();
    vinfo.xres_virtual = fbsize.width();
    vinfo.yres_virtual = 2 * fbsize.height();
    vinfo.bits_per_pixel = 32;
    vinfo.activate = FB_ACTIVATE_ALL;

    if (setFramebufferResolution(&vinfo))
      return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool DisplayManagerAML::setupScaling()
{

  fb_var_screeninfo vinfo;

  if (!getFramebufferResolution(&vinfo))
    return false;

  DMVideoModePtr currentMode = getCurrentVideoMode(0);

  if (currentMode)
  {
    QString framebufferAaxis = QString("0 0 %1 %2").arg(vinfo.xres - 1).arg(vinfo.yres - 1);
    QString screenAxis = QString("0 0 %1 %2").arg(currentMode->m_width - 1).arg(currentMode->m_height - 1);

    AMLUtils::sysfsWriteString("/sys/class/graphics/fb0/free_scale", "0");
    AMLUtils::sysfsWriteString("/sys/class/graphics/fb0/free_scale_axis", framebufferAaxis);
    AMLUtils::sysfsWriteString("/sys/class/graphics/fb0/window_axis", screenAxis);
    AMLUtils::sysfsWriteString("/sys/class/graphics/fb0/scale_width", QString::number(currentMode->m_width));
    AMLUtils::sysfsWriteString("/sys/class/graphics/fb0/scale_height", QString::number(currentMode->m_height));
    AMLUtils::sysfsWriteString("/sys/class/graphics/fb0/free_scale", "0x10001");

    QLOG_DEBUG() << QString("Setting up scaling %1x%2 -> %3x%4").arg(vinfo.xres) \
                                                                .arg(vinfo.yres) \
                                                                .arg(currentMode->m_width) \
                                                                .arg(currentMode->m_height);
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool DisplayManagerAML::setDisplayMode(int display, int mode)
{
  if (!isValidDisplayMode(display, mode) && !modeStrings[mode].isEmpty())
    return false;

  QFile file("/sys/class/display/mode");

  if (!file.open(QIODevice::ReadWrite))
    return false;

  QTextStream out(&file);
  out << modeStrings[mode];

  file.close();

  // adjust scaling if needed
  setupScaling();

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int DisplayManagerAML::getCurrentDisplayMode(int display)
{
  QFile file("/sys/class/display/mode");

  if(!file.open(QIODevice::ReadOnly))
  {
    return false;
  }

  QTextStream in(&file);
  QString currentmode = in.readLine();

  for(int modeid=0; modeid < modeStrings.size(); modeid++)
  {
    if (currentmode == modeStrings[modeid])
      return modeid;
  }

  return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int DisplayManagerAML::getMainDisplay()
{
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int DisplayManagerAML::getDisplayFromPoint(int x, int y)
{
  return 0;
}

