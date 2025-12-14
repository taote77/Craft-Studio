#ifndef _QBUS_DISPATCHER_H_
#define _QBUS_DISPATCHER_H_

#include <qbus/qbus_global.h>
#include <qbus/service_event.h>

#include <QHash>
#include <QObject>
#include <QString>

namespace qbus
{

using NotifyHandler = std::function<void(const QVariant&)>;

using RequestHandler = std::function<QVariant(const QVariant&)>;

class QBUS_API Dispatcher : public QObject
{
  Q_OBJECT
public:
  explicit Dispatcher(QObject* parent = nullptr);

  void BindNotify(const QString& topic, NotifyHandler handler);

  void BindRequest(const QString& topic, RequestHandler handler);

  void HandleNotify(const Event& msg);

private:
  QHash<QString, NotifyHandler> _notify_handlers;
  QHash<QString, RequestHandler> _request_handlers;
};

}

#endif //_QBUS_DISPATCHER_H_