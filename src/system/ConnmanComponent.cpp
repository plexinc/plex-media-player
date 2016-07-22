#include "QsLog.h"
#include "ConnmanComponent.h"

/////////////////////////////////////////////////////////////////////////////////////////
ConnmanComponent::ConnmanComponent(QObject* parent) : ComponentBase(parent), m_networkManager(nullptr), m_userAgent(nullptr)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
bool ConnmanComponent::componentInitialize()
{
  m_networkManager = NetworkManagerFactory::createInstance();
  if (!m_networkManager)
  {
    QLOG_ERROR() << "Unable to create network manager instance";
    return false;
  }

  connect(m_networkManager, &NetworkManager::technologiesChanged, this,
          &ConnmanComponent::managerTechnologiesChanged);

  connect(m_networkManager, &NetworkManager::servicesListChanged, this,
          &ConnmanComponent::managerServiceListChanged);

  m_userAgent = new UserAgent(this);
  if (!m_userAgent)
  {
    QLOG_ERROR() << "Unable to create user agent";
    return false;
  }

  connect(m_userAgent, &UserAgent::userInputRequested, this,
          &ConnmanComponent::agentUserInputRequested);

  m_networkManager->registerAgent(m_userAgent->path());

  logInfo();

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
QStringList ConnmanComponent::getTechnologies()
{
  return m_networkManager->technologiesList();
}

/////////////////////////////////////////////////////////////////////////////////////////
bool ConnmanComponent::enableTechnology(QString technology, bool enable)
{
  NetworkTechnology *tech = m_networkManager->getTechnology(technology);
  if (tech)
  {
    QLOG_DEBUG() << "Setting technology" << technology << " enable to" << enable;
    tech->setPowered(enable);
    return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool ConnmanComponent::isTechnologyEnabled(QString technology)
{
  NetworkTechnology *tech = m_networkManager->getTechnology(technology);
  if (tech)
  {
    return tech->powered();
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
QStringList ConnmanComponent::getServices(QString technology)
{
  QStringList services;
  foreach(NetworkService *serv, m_networkManager->getServices(technology))
  {
    services << serv->name();
  }
  return services;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool ConnmanComponent::scan(QString technology)
{
  NetworkTechnology *tech = m_networkManager->getTechnology(technology);
  if (tech)
  {
    QLOG_DEBUG() << "Starting scan for technology" << technology;
    tech->scan();
    return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool ConnmanComponent::connectService(QString technology, QString service)
{
  NetworkService *serv = getServiceForTechnology(technology, service);
  if (serv)
  {
    QLOG_DEBUG() << "Connecting to service" << service << "for technology " << technology;
    serv->requestConnect();
    return true;
  }
  else
  {
    QLOG_ERROR() << "Invalid connect request : service" << service << "or technology" << technology << "is unknown";
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool ConnmanComponent::isServiceConnected(QString technology, QString service)
{

  NetworkService *serv =  getServiceForTechnology(technology, service);
  if (serv)
  {
    return serv->connected();
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Usefull fields for setting a Configuration
/// The variantMap at first level should have elemnts with the following keys
///  "ipv4config" : allows to define the ip configuration, the associated value should be
///                 a VariantMap which has the following key/values pairs
///                 "Address" : "192.168.1.13"
///                 "Gateway" : "192.168.1.1"
///                 "Method" : "dhcp" / "manual"
///                 "Netmask" : "255.255.255.0"
/// "nameserversconfig" : The value should be a list of string with the nameservers IPs if any
/////////////////////////////////////////////////////////////////////////////////////////
bool ConnmanComponent::setServiceConfig(QString technology, QString service, QVariantMap options)
{
  NetworkService *serv =  getServiceForTechnology(technology, service);
  if (serv)
  {
    if (options.contains("ipv4config"))
    {
      if (options["ipv4config"].type() == QVariant::Map)
      {
        serv->setIpv4Config(options["ipv4config"].toMap());
      }
      else
      {
        QLOG_ERROR() << "Invalid IPv4 configuration type";
        return false;
      }
    }

    if (options.contains("nameserversconfig"))
    {
      if (options["nameserversconfig"].type() == QVariant::StringList)
      {
        serv->setNameserversConfig(options["nameserversconfig"].toStringList());
      }
      else
      {
        QLOG_ERROR() << "Invalid nameservers configuration type";
        return false;
      }
    }

    if (options.contains("proxyconfig"))
    {
      if (options["proxyconfig"].type() == QVariant::Map)
      {
        serv->setProxyConfig(options["proxyconfig"].toMap());
      }
      else
      {
        QLOG_ERROR() << "Invalid proxy configuration type";
        return false;
      }
    }

    return true;
  }


  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
QVariantMap ConnmanComponent::getServiceConfig(QString technology, QString service)
{
  QVariantMap Options;

  NetworkService *serv =  getServiceForTechnology(technology, service);
  if (serv)
  {
    Options["ipv4"] = serv->ipv4();
    Options["ipv4config"] = serv->ipv4Config();
    Options["nameserversconfig"] = serv->nameservers();
    Options["proxyconfig"] = serv->proxyConfig();

    return Options;
  }

  return QVariantMap();
}

/////////////////////////////////////////////////////////////////////////////////////////
bool ConnmanComponent::disconnectService(QString technology, QString service)
{

  NetworkService *serv = getServiceForTechnology(technology, service);
  if (serv)
  {
    QLOG_DEBUG() << "Disconnecting from service" << service << "for technology " << technology;
    serv->requestDisconnect();
    return true;
  }
  else
  {
    QLOG_ERROR() << "Invalid disconnect request : service" << service << "or technology" << technology << "is unknown";
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::managerTechnologiesChanged()
{
  QLOG_DEBUG() << "Technologies list changed, updating...";
  foreach(NetworkTechnology *tech, m_networkManager->getTechnologies())
  {
    connect(tech, &NetworkTechnology::poweredChanged, this,
            &ConnmanComponent::technologyPoweredChanged);
    connect(tech, &NetworkTechnology::scanFinished, this,
            &ConnmanComponent::technologyScanFinished);
    connect(tech, &NetworkTechnology::connectedChanged, this,
            &ConnmanComponent::technologyConnectedChanged);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::managerServiceListChanged()
{
  QLOG_DEBUG() << "Manager signaled service list change, updating ...";
  foreach(NetworkService *serv, m_networkManager->getServices())
  {
    HookServiceEvents(serv);
  }

  foreach(NetworkTechnology *tech,m_networkManager->getTechnologies())
  {
    emit serviceListChanged(tech->name(),getServices(tech->name()));

    if (tech->name().toLower() == CONNMAN_TECH_WIFI)
        emit wifiNetworkListChanged(getServices(CONNMAN_TECH_WIFI));
  }

  QVariantMap map;
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::agentUserInputRequested(const QString &servicePath, const QVariantMap &fields)
{
  QVariantMap reply;
  NetworkService *service = getServiceFromPath(servicePath);
  NetworkTechnology *tech = getTechnologyForService(service);

  if (!service || !tech)
  {
    QLOG_ERROR() << "Agent user input : can't match proper service/technology for " << servicePath;
    return;
  }

  QLOG_DEBUG() << "Requesting password for technology" << tech->name() << ", service" << service->name();
  emit requestServicePassword(tech->name(), service->name());
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::provideServicePassword(QString technology, QString service, QString password)
{
  QVariantMap reply;

  reply.insert("Passphrase", password);

  QLOG_DEBUG() << "Setting password to" << password << "for technology" << technology << ", service " << service;
  m_userAgent->sendUserReply(reply);
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::technologyPoweredChanged(const bool &powered)
{
  NetworkTechnology *tech = dynamic_cast<NetworkTechnology*>(sender());
  if (tech)
  {
    QLOG_DEBUG() << "Enable State changed for technology" << tech->name() << "( set to " << powered << ")";
    emit enableStateChanged(tech->name(), powered);
    if (tech->name().toLower() == CONNMAN_TECH_WIFI)
      emit wifiEnableChanged();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::technologyConnectedChanged(const bool &connected)
{
  NetworkTechnology *tech = dynamic_cast<NetworkTechnology*>(sender());
  if (tech)
  {
    QLOG_DEBUG() << "Connected state changed for technology" << tech->name() << "( set to " << connected << ")";
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::technologyScanFinished()
{
  NetworkTechnology *tech = dynamic_cast<NetworkTechnology*>(sender());
  if (tech)
  {
    QStringList services = m_networkManager->servicesList(tech->name());
    QLOG_DEBUG() << "Scan finished for technology" << tech->name() << "( " << services.size() << "services )";

    // we have to hook all services events
    foreach(NetworkService *serv, m_networkManager->getServices(tech->name()))
    {
      HookServiceEvents(serv);
    }

    emit serviceListChanged(tech->name(), services);

    if (tech->name().toLower() == CONNMAN_TECH_WIFI)
    {
      emit wifiScanCompleted();
      emit wifiNetworkListChanged(getServices(CONNMAN_TECH_WIFI));
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::serviceConnectedChanged(const bool &connected)
{
  NetworkService *serv = dynamic_cast<NetworkService*>(sender());
  if (serv)
  {
    NetworkTechnology *tech = getTechnologyForService(serv);
    if (tech)
    {
      QLOG_DEBUG() << "Connection state changed for technology" << tech->name() << ", service" << serv->name() << "changed to " << connected;
      emit connectionStateChanged(tech->name(), serv->name(), connected);

      if (tech->name().toLower() == CONNMAN_TECH_WIFI)
      {
        emit wifiConnectedChanged(serv->name(), connected);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::serviceConfigChanged()
{
  NetworkService *serv = dynamic_cast<NetworkService*>(sender());
  if (serv)
  {
    NetworkTechnology *tech = getTechnologyForService(serv);
    if (tech)
    {
      QLOG_DEBUG() << "Config  changed for technology" << tech->name() << ", service" << serv->name();
      emit serviceConfigurationChanged(tech->name(), serv->name());
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
NetworkService *ConnmanComponent::getServiceForTechnology(QString technology, QString service)
{
  const QVector<NetworkService*> services = m_networkManager->getServices(technology);

  foreach (NetworkService* serv, services)
  {
    if (serv->name() == service)
      return serv;
  }

  return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
NetworkTechnology *ConnmanComponent::getTechnologyForService(NetworkService *service)
{
  QString techpath = m_networkManager->technologyPathForService(service->path());

  foreach(NetworkTechnology *tech, m_networkManager->getTechnologies())
  {
    if (tech->path() == techpath)
      return tech;
  }

  return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
QString ConnmanComponent::getServiceNameFromPath(QString path)
{
  foreach(NetworkService *serv, m_networkManager->getServices())
  {
    if (serv->path() == path)
      return serv->name();
  }

  return "";
}

/////////////////////////////////////////////////////////////////////////////////////////
NetworkService *ConnmanComponent::getServiceFromPath(QString path)
{
  foreach(NetworkService *serv, m_networkManager->getServices())
  {
    if (serv->path() == path)
      return serv;
  }

  return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::HookServiceEvents(NetworkService *service)
{
  connect(service, &NetworkService::connectedChanged, this, &ConnmanComponent::serviceConnectedChanged);
  connect(service, &NetworkService::ipv4Changed, this, &ConnmanComponent::serviceConfigChanged);
  connect(service, &NetworkService::nameserversChanged, this, &ConnmanComponent::serviceConfigChanged);
  connect(service, &NetworkService::proxyChanged, this, &ConnmanComponent::serviceConfigChanged);
}

/////////////////////////////////////////////////////////////////////////////////////////
void ConnmanComponent::logInfo()
{
  ConnmanComponent *connman = & ConnmanComponent::Get();
  QStringList techlist = connman->getTechnologies();

  QLOG_DEBUG() << "List of technologies :";
  foreach(QString tech, techlist)
  {
    QLOG_DEBUG() << "Technology " << tech << ", Enabled :" << connman->isTechnologyEnabled(tech);

    QLOG_DEBUG() << "-> Services :";
    foreach(QString serv, connman->getServices(tech))
    {
      QLOG_DEBUG() << "\tService " << serv << "Connected :" << connman->isServiceConnected(tech,serv);

      if (connman->isServiceConnected(tech,serv))
      {
        QVariantMap props = connman->getServiceConfig(tech, serv);

        foreach(QString key, props.keys())
        {
          QLOG_DEBUG() << "\t\t" << key << ":";

          switch(props[key].type())
          {
            case QVariant::Map:
              foreach(QString subkey, props[key].toMap().keys())
              {
                QLOG_DEBUG() << "\t\t\t* " << subkey << ":" << props[key].toMap()[subkey].toString();
              }

              break;

           case QVariant::StringList:
              foreach(QString s, props[key].toStringList())
              {
                QLOG_DEBUG() << "\t\t\t* " << s;
              }

              break;

            default:
              break;
          }
        }
      }
    }
  }
}
