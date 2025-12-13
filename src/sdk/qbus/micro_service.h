#ifndef QBUS_MICRO_SERVICE
#define QBUS_MICRO_SERVICE

#include "qbus_global.h"
#include <QObject>

namespace qbus
{

class QBUS_API MicroService : public QObject
{
  Q_OBJECT
public:
  // MicroService();
  // virtual ~MicroService();

  virtual QString serviceName() const = 0;

public slots:
  virtual void init();

  virtual void startup();

  virtual void cleanup();

protected:
  // 发生广播消息
  void publish(const QString& topic, const QVariant& data);

  // 订阅广播消息
  void subscribe(const QString& topic, bool invokable = false);

  // 取消订阅广播消息
  void unsubscribe(const QString& topic, bool invokable = false);

private:
signals:
  void sigPub(const QString& topic, const QVariant& data);

  void sigSub(const QString& topic);
};

} // namespace qbus

#endif // QBUS_MICRO_SERVICE