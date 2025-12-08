#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <QDir>
#include <QMetaObject>
#include <QObject>
#include <qbus/micro_service.h>
#include <vector>

namespace qbus
{

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
  std::vector<const QMetaObject*> _services_meta;
};

} // qbus

#endif // SERVICE_MANAGER_H