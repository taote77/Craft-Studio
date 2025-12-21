#ifndef QBUS_MICRO_SERVICE
#define QBUS_MICRO_SERVICE

#include <QObject>

#include "qbus_global.h"
#include <memory>
#include <qbus/dispatcher.h>
#include <qbus/service_event.h>

#include <functional>
#include <type_traits>

namespace qbus
{

using NotifyHandler = std::function<void(const QVariant&)>;

using RequestHandler = std::function<QVariant(const QVariant&)>;

class QBUS_API MicroService : public QObject
{
  Q_OBJECT
public:
  MicroService();

  virtual QString serviceName() const = 0;

public slots:
  virtual void init();

  virtual void startup();

  virtual void cleanup();

  void handleNotify(const Event& event);

protected:
  // 注册并处理广播消息
  void registerNotifyHandler(const QString& topic, NotifyHandler handler);

  void registerRequestHandler(const QString& topic, RequestHandler handler);

  // 发生广播消息
  void publish(const QString& topic, const QVariant& data);

  // 订阅广播消息
  void subscribe(const QString& topic, bool invokable = false);

  // 取消订阅广播消息
  void unsubscribe(const QString& topic, bool invokable = false);

private:
  Dispatcher _dispatcher;

signals:
  void sigPub(const Event& event);

  // 订阅广播消息
  void sigSub(const Event& event);

  void sigUnsub(const Event& event);

  // 响应发布消息
  void onPub(const Event& event);
};

} // namespace qbus

#endif // QBUS_MICRO_SERVICE