#include "service_creator.h"
#include <QDebug>

namespace qbus
{

MicroService* ServiceCreator::createService(const QMetaObject* meta_object)
{
  qDebug() << meta_object->className();
  auto obj = qobject_cast<MicroService*>(meta_object->newInstance());
  if (obj)
  {
    obj->init();
  }
  return obj;
}

} // namespace qbus