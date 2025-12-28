#include "service_bus.h"
#include "qbus/micro_service.h"
#include <QDebug>
#include <memory>
#include <qglobal.h>
#include <qobjectdefs.h>
#include <qvariant.h>

namespace qbus
{

ServiceBus* ServiceBus::instance()
{
  static ServiceBus instance;
  return &instance;
}

ServiceBus::ServiceBus(QObject* parent)
{
  Q_UNUSED(parent);
}

void ServiceBus::PostMessage(const Event& event)
{
  // Post the message to the service bus
  emit onPub(event);
}

void ServiceBus::RegisterService(const QString& service_name, std::shared_ptr<MicroService> service)
{
  _service_list[service_name] = service;
}

void ServiceBus::onSub(const Event& event)
{
  // Handle subscription events
  if (event.type == Event::Async)
  {
    qDebug() << "scribe: " << event.from << event.topic;
    _subscribers[event.topic].emplace_back(event.from);
  }
  else if (event.type == Event::Sync)
  {
    qDebug() << "add sync service interface: " << event.from << event.topic;
    _serves[event.topic] = event.from;
  }
}

void ServiceBus::onPub(const Event& event)
{
  if (event.type == Event::Async)
  {
    // Handle publish events
    if (_subscribers.contains(event.topic))
    {
      for (auto& address : _subscribers[event.topic])
      {
        auto service = _service_list[address].lock();
        if (service)
        {
          QMetaObject::invokeMethod(
            service.get(), "handleNotify", Qt::QueuedConnection, Q_ARG(Event, event));
        }
      }
    }
    else
    {
      qWarning() << "No subscribers for topic:" << event.topic;
    }
  }
}

QVariant ServiceBus::onRequest(const Event& event)
{
  if (event.type == Event::Sync)
  {
    qDebug() << Q_FUNC_INFO << event.topic << event.type << event.data;

    //   // Handle publish events
    if (_serves.contains(event.topic))
    {
      // for (auto& address : _serves[event.topic])
      // {

      auto address = _serves[event.topic];
      qDebug() << Q_FUNC_INFO << "on request" << __LINE__ << event.topic << _serves.size();
      auto service = _service_list[address].lock();
      if (service)
      {
        QVariant ret;
        bool rr = QMetaObject::invokeMethod(service.get(), "handleRequest",
          Qt::BlockingQueuedConnection, Q_RETURN_ARG(QVariant, ret), Q_ARG(Event, event));

        qDebug() << "ret===============:" << rr << ret;
        return ret;
      }
      // }
    }
    else
    {
      qWarning() << "No subscribers for topic:" << event.topic;
    }
  }

  return QVariant::fromValue(19);
}

} // namespace qbus