#ifndef _QBUS_MICRO_SERVICE_EVENT_H_
#define _QBUS_MICRO_SERVICE_EVENT_H_

#include "qbus/micro_service.h"
#include <QMetaType>
#include <QVariant>

namespace qbus
{

struct MicroServiceEvent
{
  int topic;
  QString from;
  QString to;
  QVariant data;
  QVariantList args;

  enum Type
  {
    Async,     // Publish:
    Sync,      // BlockRequest:    thread safe
    FakeSync,  // Request:         eventloop can still work
    DangerSync // Invoke:          not thread safe
  };
  Type type = MicroServiceEvent::Async;
};

} // namespace qbus

Q_DECLARE_METATYPE(qbus::MicroServiceEvent)

#endif // _QBUS_EVENT_H_