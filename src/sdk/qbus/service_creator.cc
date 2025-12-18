#include "service_creator.h"
#include <qbus/service_bus.h>

#include <QDebug>
#include <qnamespace.h>

namespace qbus
{

ServiceCreator::ServiceCreator()
{
  //
}

MicroService* ServiceCreator::createService(const QMetaObject* meta_object)
{
  qDebug() << meta_object->className();
  MicroService* obj = qobject_cast<MicroService*>(meta_object->newInstance());
  if (obj)
  {

    auto meta_con = QObject::connect(
      obj, &MicroService::sigSub, ServiceBus::instance(), &ServiceBus::onSub, Qt::UniqueConnection);

    obj->init(); // 启用订阅
  }

  return obj;
}

} // namespace qbus