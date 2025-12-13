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

#include "qbus/micro_service_event.h"
#include <QObject>
#include <qobject.h>

namespace qbus
{

class ServiceBus : public QObject
{
  Q_OBJECT
public:
  Q_INVOKABLE explicit ServiceBus(QObject* parent = nullptr);

public:
  void PostMessage(const MicroServiceEvent& event);
signals:
  void onPub(const MicroServiceEvent& event);
  void onSub(const MicroServiceEvent& event);
};

}

#endif // QBUS_SERVICE_BUS_H