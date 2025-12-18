#ifndef QBUS_SERVICE_BUS_H
#define QBUS_SERVICE_BUS_H

/**********************************************************************************
 * ServiceBus class declaration
 *
 * This class serves as a central communication hub within the QBus SDK,
 * facilitating message passing and event handling between different components.
 * registration and dispatching of events.
 * it plays a crucial role in enabling decoupled communication.

 * @class ServiceBus
 * @note  负责微服务消息的分发、订阅、发布、请求、响应、异步、同步消息等功能。
 ***********************************************************************************/

#include "qbus/micro_service.h"
#include "qbus/service_event.h"

#include <QObject>
#include <memory>
#include <qchar.h>

namespace qbus
{

class ServiceBus : public QObject
{
  Q_OBJECT

public:
  static ServiceBus* instance();

private:
  Q_INVOKABLE explicit ServiceBus(QObject* parent = nullptr);

public:
  void PostMessage(const Event& event);

  void RegisterService(const QString& service_name, std::shared_ptr<MicroService> service);

public slots:
  void onPub(const Event& event);

  void onSub(const Event& event);

private:
  std::map<QString, std::weak_ptr<MicroService>> _service_list; // service_address, service_object

  std::map<QString, std::list<QString>> _subscribers; // topic, service_address
};

}

#endif // QBUS_SERVICE_BUS_H