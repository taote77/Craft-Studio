#include "router.h"
#include <QDebug>
#include <pdk/message_bus.h>

namespace pdk
{

Router::Router(QObject* parent)
  : QObject(parent)
{
}

void Router::Attacth(Plugin* plugin)
{
  QMutexLocker lock(&mutex_);

  for (auto& obj : plugin->Objects())
  {
    QObject::connect(obj.data(), &Object::sendPub, this, &Router::OnPub);
    QObject::connect(obj.data(), &Object::sendSub, this, &Router::OnSub);
    QObject::connect(obj.data(), &Object::sendRequest, this, &Router::OnRequest);
    QObject::connect(obj.data(), &Object::sendBlockRequest, this, &Router::OnBlockRequest);

    QObject::connect(
      obj.data(), &Object::sendInvoke, this, &Router::OnInvoke, Qt::DirectConnection);

    objects_[obj->Path()] = obj.toWeakRef();
  }

  QObject::connect(MessageBus::Instance(), &MessageBus::sigPub, this, &Router::OnPub,
    Qt::ConnectionType::UniqueConnection);

  plugin->Ready();
}

void Router::Detach(Plugin* plugin)
{
  QMutexLocker lock(&mutex_);

  for (auto& obj : plugin->Objects())
  {
    obj->disconnect();
    objects_.remove(obj->Path());
  }
}

void Router::OnSub(const Message& msg)
{
  auto invokable = msg.data.toBool();

  if (invokable)
  {
    AddInvoker(msg.from, msg.topic);
  }
  else
  {
    AddSubscriber(msg.from, msg.topic);
  }
}

void Router::OnUnsub(const Message& msg)
{
  auto invokable = msg.data.toBool();

  if (invokable)
  {
    RemoveInvoker(msg.topic);
  }
  else
  {
    RemoveSubscriber(msg.from, msg.topic);
  }
}

void Router::OnPub(const Message& msg)
{
  if (!subscribers_.contains(msg.topic))
  {
    // qWarning()<<"not object subscribe to this topic"<<msg.topic;
    return;
  }

  for (auto& address : subscribers_.value(msg.topic))
  {
    auto obj = objects_.value(address);
    if (!obj.isNull())
    {
      // qDebug()<<"[ultrabus - publish] topic"<<msg.topic<<"from"<<msg.from<<"to"<<address;
      if (!msg.feedback.isEmpty())
      {
        auto ret =
          QMetaObject::invokeMethod(obj.lock().data(), "HandleFeedback", Q_ARG(Message, msg));

        RemoveSubscriber(msg.to, msg.topic); // topic there is same as request  feedback seq

        Q_ASSERT(ret);
      }
      else
      {
        auto ret =
          QMetaObject::invokeMethod(obj.lock().data(), "HandleNotify", Q_ARG(Message, msg));

        Q_ASSERT(ret);
      }
    }
  }
}

void Router::OnRequest(const Message& msg)
{

  auto obj = objects_.value(invokers_.value(msg.topic));
  if (!obj.isNull())
  {
    AddSubscriber(msg.from, msg.feedback);

    // qDebug()<<"[ultrabus - invoke] topic"<<msg.topic<<"from"<<msg.from<<"to"<<obj.lock()->Path();
    auto ret = QMetaObject::invokeMethod(obj.lock().data(), "HandleRequest", Q_ARG(Message, msg));

    Q_ASSERT(ret);
  }
  else
  {

    qWarning() << "invokable method not found:" << msg.topic;

    Message feedback;
    feedback.uuid = QUuid::createUuid();
    feedback.timestamp = QDateTime::currentMSecsSinceEpoch();
    feedback.topic = msg.feedback;
    feedback.data = InvokeError::TopicError();
    feedback.from = "pdk::Router";

    auto sender = QObject::sender();
    auto ret = QMetaObject::invokeMethod(sender, "HandleFeedback", Q_ARG(Message, feedback));

    Q_ASSERT(ret);
  }
}

void Router::OnBlockRequest(const Message& msg, QWeakPointer<MsgPromise> prom)
{
  auto obj = objects_.value(invokers_.value(msg.topic));
  if (!obj.isNull())
  {
    AddSubscriber(msg.from, msg.feedback);

    // qDebug()<<"[ultrabus - invoke] topic"<<msg.topic<<"from"<<msg.from<<"to"<<obj.lock()->Path();
    auto ret = QMetaObject::invokeMethod(obj.lock().data(), "HandleBlockRequest",
      Q_ARG(Message, msg), Q_ARG(QWeakPointer<MsgPromise>, prom));

    Q_ASSERT(ret);
  }
  else
  {

    qWarning() << "BlockRequest method not found:" << msg.topic;

    Message feedback;
    feedback.uuid = QUuid::createUuid();
    feedback.timestamp = QDateTime::currentMSecsSinceEpoch();
    feedback.topic = msg.feedback;
    feedback.data = InvokeError::TopicError();
    feedback.from = "pdk::Router";

    auto sender = QObject::sender();

    auto ret = QMetaObject::invokeMethod(
      sender, "HandleBlockRequest", Q_ARG(Message, msg), Q_ARG(QWeakPointer<MsgPromise>, prom));

    Q_ASSERT(ret);
  }
}

QVariant Router::OnInvoke(const Message& msg)
{
  auto obj = objects_.value(invokers_.value(msg.topic));
  if (!obj.isNull())
  {
    // qDebug()<<"[ultrabus - call] topic"<<msg.topic<<"from"<<msg.from<<"to"<<obj.lock()->Path();
    return obj.lock()->HandleInvoke(msg);
  }

  qWarning() << "invokable method not found:" << msg.topic;

  return QVariant();
}

void Router::AddSubscriber(const Address& address, const QString& topic)
{
  if (!subscribers_.value(topic).contains(address))
  {
    subscribers_[topic].append(address);
  }
}

void Router::AddInvoker(const Address& address, const QString& topic)
{
  if (!invokers_.contains(topic))
  {
    invokers_[topic] = address;
  }
  else
  {
    qWarning() << "Failed to add invokable method" << topic << "from" << address
               << ",method already exist on" << invokers_.value(topic);
  }
}

void Router::RemoveSubscriber(const Address& address, const QString& topic)
{
  if (topic.isEmpty())
  {
    for (auto&& t : subscribers_.keys())
    {
      subscribers_[t].removeAll(address);
      auto iter = subscribers_.find(t);
      iter->removeAll(address);
      if (iter->isEmpty())
      {
        subscribers_.erase(iter);
      }
    }
  }
  else
  {
    auto iter = subscribers_.find(topic);
    if (iter != subscribers_.end())
    {
      iter->removeAll(address);
      if (iter->isEmpty())
      {
        subscribers_.erase(iter);
      }
    }
  }
}

void Router::RemoveInvoker(const QString& topic)
{
  invokers_.remove(topic);
}

}
