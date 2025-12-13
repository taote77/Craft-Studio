#include <qbus/micro_service.h>

#include <QDebug>

namespace qbus
{
// MicroService::MicroService()
// {
//   //
//   qDebug() << Q_FUNC_INFO;
// }

// MicroService::~MicroService()
// {
//   //

//   qDebug() << Q_FUNC_INFO;
// }

void MicroService::init() {}

void MicroService::startup()
{
  //
}

void MicroService::cleanup()
{
  //
}

void MicroService::publish(const QString& topic, const QVariant& data)
{
  //
  Q_EMIT sigPub(topic, data);
}

void MicroService::subscribe(const QString& topic, bool invokable)
{
  //
  Q_EMIT sigSub(topic);
}
void MicroService::unsubscribe(const QString& topic, bool invokable)
{
  //
}

} // namespace qbus