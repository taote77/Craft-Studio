#ifndef QBUS_SERVICE_CREATOR_H
#define QBUS_SERVICE_CREATOR_H

#include <qbus/micro_service.h>

namespace qbus
{

class ServiceCreator
{
public:
  ServiceCreator();
  ~ServiceCreator();

public slots:
  MicroService* createService(const QMetaObject* meta_object);
};

} // namespace qbus
  //
#endif // QBUS_SERVICE_CREATOR_H