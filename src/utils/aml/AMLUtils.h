#ifndef AMLUTILS_H
#define AMLUTILS_H

#include <QString>

namespace AMLUtils
{
  bool sysfsWriteString(QString path, QString value);
};

#endif /* AMLUTILS_H */
