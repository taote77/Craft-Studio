#include "qbus/service_event.h"
#include <qbus/micro_service.h>

#include <QDebug>
#include <qglobal.h>
#include <qobjectdefs.h>

namespace qbus
{

MicroService::MicroService()
{
  //
  // qDebug() << Q_FUNC_INFO;
}

// MicroService::~MicroService()
// {
//   //

//   qDebug() << Q_FUNC_INFO;
// }

void MicroService::init()
{
  // _dispatcher = new Dispatcher(this);
}

void MicroService::startup()
{
  //
}

void MicroService::cleanup()
{
  //
}

void MicroService::handleNotify(const Event& event)
{
  _dispatcher.HandleNotify(event);
}

void MicroService::registerNotifyHandler(const QString& topic, NotifyHandler handler)
{
  //
  qDebug() << Q_FUNC_INFO << topic;
  _dispatcher.BindNotify(topic, handler);

  Event event;
  event.topic = topic;
  event.from = this->serviceName();
  Q_EMIT sigSub(event);
}

void MicroService::registerRequestHandler(const QString& topic, RequestHandler handler)
{
  //
  _dispatcher.BindRequest(topic, handler);
}

void MicroService::publish(const QString& topic, const QVariant& data)
{
  //
  Event event;
  event.topic = topic;
  event.data = data;
  event.type = Event::Async;
  event.from = this->serviceName();

  Q_EMIT sigPub(event);
}

void MicroService::subscribe(const QString& topic, bool invokable)
{
  //
  // Q_EMIT sigSub(topic);
  Event event;
  event.topic = topic;
  event.from = this->serviceName();
  event.type = Event::Async;
  Q_EMIT sigSub(event);
}
void MicroService::unsubscribe(const QString& topic, bool invokable)
{
  //
}

} // namespace qbus