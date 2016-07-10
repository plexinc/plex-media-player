#include "DisplayManagerAML.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QThread>

#include "QsLog.h"

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

  return DisplayManager::initialize();
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

