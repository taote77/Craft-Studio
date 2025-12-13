#include "service_creator.h"
#include <QDebug>

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
    obj->init();
  }
  return obj;
}

} // namespace qbus