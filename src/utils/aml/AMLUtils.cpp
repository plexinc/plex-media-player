
#include "AMLUtils.h"
#include <QFile>
#include <QsLog.h>

/////////////////////////////////////////////////////////////////////////////////////////
bool AMLUtils::sysfsWriteString(QString path, QString value)
{
  QFile file(path);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QLOG_ERROR() << "Failed to open " << path;
    return false;
  }

  QTextStream stream(&file);
  stream << value;
  file.close();

  return true;
}
