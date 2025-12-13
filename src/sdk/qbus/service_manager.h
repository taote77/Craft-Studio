#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

/**********************************************************************************
 * ServiceManager class declaration
 *
 * This class is responsible for managing the lifecycle of microservices within
 * the QBus SDK, including loading, starting, and stopping services.
 ***********************************************************************************/

#include <QDir>
#include <QMetaObject>
#include <QObject>
#include <QThread>
#include <QVector>

#include <qbus/micro_service.h>
#include <qbus/service_bus.h>
#include <qbus/service_creator.h>

namespace qbus
{

struct ServiceCxt
{
  QThread* thread = nullptr;
  MicroService* service = nullptr;
  ServiceCreator* creator = nullptr;
};

class ServiceManager : public QObject
{
  Q_OBJECT
public:
  ServiceManager(QObject* parent = nullptr);

  ~ServiceManager();

  void loadServices(const QDir& plugin_dir);

  void Start();

  void Stop();

  //
signals:

protected:
  QStringList getPluginsLoadOrder(const QDir& plugin_dir);

  void createServices(); // 创建插件

  bool initServices(ServiceBus* service_bus); // 初始化发布订阅，请求响应相关服务

private:
  QVector<const QMetaObject*> _services_meta; // service meta

  QHash<QString, ServiceCxt> _services;

  ServiceBus* _service_bus; // central service bus for all services
};

} // qbus

#endif // SERVICE_MANAGER_H