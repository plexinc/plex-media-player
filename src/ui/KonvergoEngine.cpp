//
// Created by Tobias Hieta on 26/02/16.
//

#include "KonvergoEngine.h"

#include "ComponentManager.h"
#include "UniqueApplication.h"

#include <QQmlContext>

/////////////////////////////////////////////////////////////////////////////////////////
KonvergoEngine::KonvergoEngine() : QObject(nullptr)
{
  KonvergoWindow::RegisterClass();

  m_engine = new QQmlApplicationEngine(this);

  m_engine->rootContext()->setContextProperty("components", &ComponentManager::Get().getQmlPropertyMap());

  // This controls how big the web view will zoom using semantic zoom
  // over a specific number of pixels and we run out of space for on screen
  // tiles in chromium. This only happens on OSX since on other platforms
  // we can use the GPU to transfer tiles directly but we set the limit on all platforms
  // to keep it consistent.
  //
  // See more discussion in: https://github.com/plexinc/plex-media-player/issues/10
  // The number of pixels here are REAL pixels, the code in webview.qml will compensate
  // for a higher DevicePixelRatio
  //
  m_engine->rootContext()->setContextProperty("webMaxHeight", 1440);

  // the only way to detect if QML parsing fails is to hook to this signal and then see
  // if we get a valid object passed to it. Any error messages will be reported on stderr
  // but since no normal user should ever see this it should be fine
  //
  connect(m_engine, &QQmlApplicationEngine::objectCreated, [=](QObject* object, const QUrl& url)
  {
    Q_UNUSED(url);

    if (object == 0)
      throw FatalException(QObject::tr("Failed to parse application engine script."));

    QObject* rootObject = m_engine->rootObjects().first();

    QObject* webChannel = qvariant_cast<QObject*>(rootObject->property("webChannel"));
    Q_ASSERT(webChannel);
    ComponentManager::Get().setWebChannel(qobject_cast<QWebChannel*>(webChannel));
  });
}

/////////////////////////////////////////////////////////////////////////////////////////
void KonvergoEngine::loadUI()
{
  m_engine->load(QUrl(QStringLiteral("qrc:/ui/webview.qml")));
}

/////////////////////////////////////////////////////////////////////////////////////////
KonvergoWindow* KonvergoEngine::GetWindow()
{
  KonvergoWindow* window = qobject_cast<KonvergoWindow*>(KonvergoEngine::Get().qmlEngine()->rootObjects().first());

  return window;
}

/////////////////////////////////////////////////////////////////////////////////////////
void KonvergoEngine::unload()
{
  delete m_engine;
  m_engine = nullptr;
}


