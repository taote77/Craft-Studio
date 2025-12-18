#ifndef _QBUS_MICRO_SERVICE_EVENT_H_
#define _QBUS_MICRO_SERVICE_EVENT_H_

// #include "qbus/micro_service.h"
#include <QMetaType>
#include <QVariant>
#include <functional>

namespace qbus
{

using NotifyHandler = std::function<void(const QVariant&)>;

using RequestHandler = std::function<QVariant(const QVariant&)>;

struct Event
{
  QString topic;
  QString from; // source service
  QString to;   // target service
  QVariant data;
  QVariantList args;

  enum Type
  {
    Async, // Publish:
    Sync,  // BlockRequest:
  };
  Type type = Event::Async;
};

} // namespace qbus

Q_DECLARE_METATYPE(qbus::Event)

#endif // _QBUS_EVENT_H_