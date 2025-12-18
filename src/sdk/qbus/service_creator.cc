#include "service_creator.h"
#include <qbus/service_bus.h>

#include <QDebug>

namespace qbus
{

ServiceCreator::ServiceCreator()
{
  //
}

std::shared_ptr<MicroService> ServiceCreator::createService(const QMetaObject* meta_object)
{
  qDebug() << meta_object->className();
  MicroService* obj = qobject_cast<MicroService*>(meta_object->newInstance());
  if (obj)
  {

    auto meta_con =
      QObject::connect(obj, &MicroService::sigSub, ServiceBus::instance(), &ServiceBus::onSub);

    obj->init();
  }

  std::shared_ptr<MicroService> service(obj);

  return service;
}

} // namespace qbus