#include "service_bus.h"
#include "qbus/micro_service.h"
#include <memory>
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

void ServiceBus::onSub(const Event& event, std::shared_ptr<MicroService> sender)
{
  // Handle subscription events
  if (event.type == Event::Async)
  {
    _subscribers[event.topic] = sender;
  }
}

void ServiceBus::onPub(const Event& event)
{
  if (event.type == Event::Async)
  {
    // Handle publish events
    if (_subscribers.contains(event.topic))
    {
      QMetaObject::invokeMethod(_subscribers[event.topic].lock().get(), "handleNotify",
        Qt::QueuedConnection, Q_ARG(Event, event));
      // _subscribers[event.topic]->handleNotify(event);
    }
  }
}

} // namespace qbus