#include "dispatcher.h"

#include <utility>

namespace qbus
{

Dispatcher::Dispatcher(QObject* parent)
  : QObject(parent)
{
}

// QHash<QString, std::function<NotifyHandler>> _notify_handlers;
// QHash<QString, std::function<RequestHandler>> _request_handlers;

void Dispatcher::BindNotify(const QString& topic, NotifyHandler handler)
{
  _notify_handlers.insert(topic, std::move(handler));
}

void Dispatcher::BindRequest(const QString& topic, RequestHandler handler)
{
  _request_handlers.insert(topic, std::move(handler));
}

void Dispatcher::HandleNotify(const Event& msg)
{

  if (_notify_handlers.contains(msg.topic))
  {
    _notify_handlers[msg.topic](msg.data);
  }
}

}
