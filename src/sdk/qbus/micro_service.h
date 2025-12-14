#ifndef QBUS_MICRO_SERVICE
#define QBUS_MICRO_SERVICE

#include <QObject>

#include "qbus_global.h"
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
  virtual QString serviceName() const = 0;

public slots:
  virtual void init();

  virtual void startup();

  virtual void cleanup();

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
  Dispatcher* _dispatcher;

signals:
  void sigPub(const QString& topic, const QVariant& data);

  void sigSub(const QString& topic);

  void onPub(const QString& topic, const QVariant& data);
};

} // namespace qbus

#endif // QBUS_MICRO_SERVICE