#ifndef QBUS_SERVICE_CREATOR_H
#define QBUS_SERVICE_CREATOR_H

#include <QObject>
#include <qbus/micro_service.h>

namespace qbus
{

class ServiceCreator : public QObject
{
  Q_OBJECT
public:
  ServiceCreator();
  // ~ServiceCreator();

public slots:
  std::shared_ptr<MicroService> createService(const QMetaObject* meta_object);
};

} // namespace qbus
  //
#endif // QBUS_SERVICE_CREATOR_H