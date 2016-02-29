#ifndef KONVERGOENGINE_H
#define KONVERGOENGINE_H

#include <QQmlApplicationEngine>
#include "utils/Utils.h"

class KonvergoEngine : public QObject
{
  Q_OBJECT
  DEFINE_SINGLETON(KonvergoEngine)

public:
  KonvergoEngine();

  void loadUI();
  QQmlApplicationEngine* qmlEngine() const
  {
    return m_engine;
  }

  void unload();

  static KonvergoWindow* GetWindow();

private:
  bool m_isLoaded;

  QQmlApplicationEngine* m_engine;
};

#endif // KONVERGOENGINE_H
