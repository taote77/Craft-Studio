#include "service_bus.h"
#include "qbus/micro_service.h"
#include <QDebug>
#include <memory>
#include <qglobal.h>
#include <qobjectdefs.h>

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
    _subscribers[event.topic].emplace_back(event.from);
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
        // invokemethod
        auto service = _service_list[address].lock();
        if (service)
        {
          QMetaObject::invokeMethod(
            service.get(), "handleNotify", Qt::QueuedConnection, Q_ARG(Event, event));
        }
      }
    }
  }
}

} // namespace qbus