#include "service_bus.h"

namespace qbus
{

ServiceBus::ServiceBus(QObject* parent)
{
  Q_UNUSED(parent);
}

void ServiceBus::PostMessage(const MicroServiceEvent& event)
{
  // Post the message to the service bus
  emit onPub(event);
}
} // namespace qbus