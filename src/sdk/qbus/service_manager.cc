#include <qbus/service_export.h>
#include <qbus/service_manager.h>

#include <QDebug>
#include <QLibrary>
#include <QMetaObject>
#include <QObject>
#include <QThreadPool>
#include <qglobal.h>

#include <qbus/service_bus.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qthread.h>

namespace qbus
{

using ServicePtr = std::shared_ptr<MicroService>;
ServiceManager::ServiceManager(QObject* parent)
  : QObject(parent)
{
  qRegisterMetaType<MicroService*>("MicroService*");
  qRegisterMetaType<const QMetaObject*>("const QMetaObject*");
  qRegisterMetaType<ServicePtr>("ServicePtr");
  qRegisterMetaType<Event>("Event");

  auto threadpool = QThreadPool::globalInstance();
  constexpr int kMaxThreadCount = 10;
  if (threadpool->maxThreadCount() < kMaxThreadCount)
  {
    qDebug() << "default QThreadPool count:" << threadpool->maxThreadCount();
    threadpool->setMaxThreadCount(kMaxThreadCount);
    qDebug() << "set QThreadPool count:" << threadpool->maxThreadCount();
  }

  ServiceBus::instance(); // initialize singleton instance

  qDebug() << "ServiceManager initd ...";
}

ServiceManager::~ServiceManager()
{
  //
}

void ServiceManager::loadServices(const QDir& plugin_dir)
{
  for (const auto& filename : getPluginsLoadOrder(plugin_dir))
  {
    qDebug() << "load plugin:" << plugin_dir.absoluteFilePath(filename);

    QLibrary plugin(plugin_dir.absoluteFilePath(filename), this);
    if (plugin.load())
    {
      auto get_services_meta = reinterpret_cast<plugin_api_t>(plugin.resolve("getServicesMeta"));
      if (get_services_meta)
      {
        PluginExport plugin_export;
        get_services_meta(&plugin_export);
        for (const QMetaObject* meta_obj : plugin_export.meta_objects)
        {
          _services_meta.push_back(meta_obj);
        }
      }
      else
      {
        qCritical() << "get_services_meta failed";
      }
    }
    else
    {
      qCritical() << "load plugin failed," << plugin.fileName() << "," << plugin.errorString();
    }
  }

  createServices(); // create service objects
  //
  initServices();
}

QStringList ServiceManager::getPluginsLoadOrder(const QDir& plugin_dir)
{
  QStringList load_order;
  QStringList filters;
  filters << "*.mx";
  for (const auto& file : plugin_dir.entryList(filters))
  {
    load_order.push_back(plugin_dir.absoluteFilePath(file));
  }

  qDebug() << "plugins load order: " << load_order;
  return load_order;
}

void ServiceManager::createServices()
{
  for (const QMetaObject* meta_obj : _services_meta)
  {
    ServiceCxt service_cxt;
    service_cxt.thread = new QThread();
    service_cxt.thread->setObjectName(meta_obj->className());
    service_cxt.thread->start();
    service_cxt.creator = new ServiceCreator();
    service_cxt.creator->moveToThread(service_cxt.thread);

    MicroService* service_obj = nullptr;

    bool ret =
      QMetaObject::invokeMethod(service_cxt.creator, "createService", Qt::BlockingQueuedConnection,
        Q_RETURN_ARG(MicroService*, service_obj), Q_ARG(const QMetaObject*, meta_obj));

    qDebug() << "create service:" << meta_obj->className() << "," << ret;

    if (!service_obj)
    {
      qCritical() << "create service object failed";
      continue;
    }

    service_cxt.service.reset(service_obj);

    ServiceBus::instance()->RegisterService(service_obj->serviceName(), service_cxt.service);

    auto meta_con = QObject::connect(service_obj, &MicroService::sigPub, ServiceBus::instance(),
      &ServiceBus::onPub, Qt::UniqueConnection);

    _services[service_obj->serviceName()] = service_cxt;
  }
}

bool ServiceManager::initServices()
{
  {
    for (auto& service_cxt : _services)
    {
      if (service_cxt.service == nullptr)
      {
        qWarning() << "Service object is null!";
        continue;
      }
    }
  }

  for (auto& service_cxt : _services)
  {
    if (service_cxt.service == nullptr)
    {
      qWarning() << "Service object is null!";
      continue;
    }

    qDebug() << Q_FUNC_INFO << service_cxt.service->serviceName() << QThread::currentThread();

    QMetaObject::invokeMethod(service_cxt.service.get(), "startup", Qt::BlockingQueuedConnection);
  }

  return true;
}

void ServiceManager::Start()
{
  //
}

void ServiceManager::Stop()
{
  //
}
}