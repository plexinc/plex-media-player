#ifndef CONNMANCOMPONENT_H
#define CONNMANCOMPONENT_H

#include <QObject>
#include <QStringList>
#include <ComponentManager.h>

#include "connman-qt5/networkmanager.h"
#include "connman-qt5/useragent.h"

#define CONNMAN_TECH_WIFI "wifi"

class ConnmanComponent : public ComponentBase
{
  Q_OBJECT
  DEFINE_SINGLETON(ConnmanComponent);

public:

  virtual bool componentExport() { return true; }
  virtual const char* componentName() { return "network"; }
  virtual bool componentInitialize();

  // Webclient API
  Q_INVOKABLE bool hasWifi() { return getTechnologies().contains(CONNMAN_TECH_WIFI); }
  Q_INVOKABLE bool enableWifi() { return enableTechnology(CONNMAN_TECH_WIFI, true); }
  Q_INVOKABLE bool disableWifi() { return enableTechnology(CONNMAN_TECH_WIFI, false); }
  Q_INVOKABLE bool isWifiEnabled() { return isTechnologyEnabled(CONNMAN_TECH_WIFI); }
  Q_INVOKABLE bool connectWifi(QString network) { return connectService(CONNMAN_TECH_WIFI, network); }
  Q_INVOKABLE void disconnectWifi(QString network) { disconnectService(CONNMAN_TECH_WIFI, network); }
  Q_INVOKABLE void provideWifiPassword(QString network, QString password) { provideServicePassword(CONNMAN_TECH_WIFI, network, password); }
  Q_INVOKABLE void scanWifi() { scan(CONNMAN_TECH_WIFI); }

  // utilities functions
  Q_INVOKABLE void logInfo();

Q_SIGNALS:
  // Webclient events
  void wifiEnableChanged(bool enabled);
  void wifiScanCompleted();
  void wifiConnectedChanged(QString network,  bool connected);
  void wifiNetworkListChanged(QStringList networks);
  void wifiPasswordRequired(QString network);

  // Generic events
  void enableStateChanged(QString technology, bool enabled);
  void serviceListChanged(QString technology, QStringList services);
  void connectionStateChanged(QString technology, QString service, bool connected);
  void serviceConfigurationChanged(QString technology, QString service);
  void requestServicePassword(QString technology, QString service);

private:
  ConnmanComponent(QObject* parent = 0);

  NetworkManager* m_networkManager;
  UserAgent* m_userAgent;
  QVector<NetworkTechnology *> m_technologies;
  QVector<NetworkService*> m_services;

  // technologies related functions
  QStringList getTechnologies();
  bool enableTechnology(QString technology, bool state);
  bool isTechnologyEnabled(QString technology);

  // services related functions
  QStringList getServices(QString technology);
  bool scan(QString technology);
  bool connectService(QString technology, QString service);
  bool isServiceConnected(QString technology, QString service);
  bool setServiceConfig(QString technology, QString service, QVariantMap options);
  QVariantMap getServiceConfig(QString technology, QString service);
  bool disconnectService(QString technology, QString service);
  void provideServicePassword(QString technology, QString service, QString password);

  NetworkService *getServiceForTechnology(QString technology, QString service);
  NetworkTechnology *getTechnologyForService(NetworkService *service);
  QString getServiceNameFromPath(QString path);
  NetworkService* getServiceFromPath(QString path);
  void HookTechnologiesEvents();
  void HookServicesEvents();

private slots:
  // manager events
  void managerTechnologiesChanged();
  void managerServiceListChanged();

  // technology events
  void technologyPoweredChanged(const bool& powered);
  void technologyScanFinished();
  void technologyConnectedChanged(const bool& connected);


  // service events
  void serviceConnectedChanged(const bool& connected);
  void serviceConfigChanged();

  // agent events
   void agentUserInputRequested(const QString& servicePath, const QVariantMap& fields);
};

#endif // CONNMANCOMPONENT_H
