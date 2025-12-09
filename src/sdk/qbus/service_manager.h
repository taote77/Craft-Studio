#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <QDir>
#include <QMetaObject>
#include <QObject>
#include <QVector>

#include <qbus/micro_service.h>
#include <qbus/service_creator.h>
#include <qthread.h>

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
  ServiceManager();

  ~ServiceManager();

  void loadServices(const QDir& plugin_dir);

  void Start();

  void Stop();

  //
signals:

protected:
  QStringList getPluginsLoadOrder(const QDir& plugin_dir);

private:
  QVector<const QMetaObject*> _services_meta;
};

} // qbus

#endif // SERVICE_MANAGER_H