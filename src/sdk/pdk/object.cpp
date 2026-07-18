#include "object.h"
#include "dispatcher.h"

#include <QEventLoopLocker>
#include <QLoggingCategory>
#include <QMessageLogger>
#include <QMetaType>
#include <QThread>
namespace pdk
{

Object::Object()
  : dispatcher_(new Dispatcher(this))
{
  qRegisterMetaType<QWeakPointer<MsgPromise>>("QWeakPointer<MsgPromise>");
  //    qRegisterMetaType<QWeakPointer<pdk::MsgPromise>>("QWeakPointer<pdk::PromiseMsg>");
}

Object::~Object() {}

Address Object::Path() const
{
  return property("ObjectPath").toString();
}

Object::ObjectFlag Object::Flag() const
{
  return ObjectFlag(property("ObjectFlag").toInt());
}

void Object::Subscribe(const QString& topic, bool invokable)
{
  Message msg;
  msg.uuid = QUuid::createUuid();
  msg.timestamp = QDateTime::currentMSecsSinceEpoch();
  msg.topic = topic;
  msg.data = invokable;
  msg.from = Path();

  emit sendSub(msg);
}

void Object::Subscribe(const int& topic, bool invokable)
{
  Subscribe(QString::number(topic), invokable);
}

void Object::Unsubscribe(const QString& topic, bool invokable)
{
  Message msg;
  msg.uuid = QUuid::createUuid();
  msg.timestamp = QDateTime::currentMSecsSinceEpoch();
  msg.topic = topic;
  msg.data = invokable;
  msg.from = Path();

  emit sendUnsub(msg);
}

void Object::Unsubscribe(const int& topic, bool invokable)
{
  Unsubscribe(QString::number(topic), invokable);
}

void Object::Publish(const QString& topic, const QVariant& data)
{
  Message msg;
  msg.uuid = QUuid::createUuid();
  msg.timestamp = QDateTime::currentMSecsSinceEpoch();
  msg.topic = topic;
  msg.data = data;
  msg.from = Path();
  msg.type = Message::Async;

  emit sendPub(msg);
}

void Object::Publish(const int& topic, const QVariant& data)
{
  Publish(QString::number(topic), data);
}

void Object::Publish(const int& topic, const google::protobuf::Message& data)
{
  Publish(topic, QByteArray::fromStdString(data.SerializeAsString()));
}

PODReply Object::RequestImpl(Message msg, int timeout)
{
  QThread::msleep(1);

  dispatcher_->TrackFeedback(msg.feedback);

  emit sendRequest(msg);

  auto reply = dispatcher_->WaitForFeedback(msg.feedback, timeout);

  PODReply data;
  data.data = reply->Data();
  data.error_ = reply->Error().Code();
  data.message_ = reply->Error().Message();

  return data;
}

ReplyPtr Object::Request(const QString& topic, const QVariant& data, int timeout)
{
  const char* k_mem_func = "RequestImpl";
  QThread::msleep(1);

  Message msg;
  msg.uuid = QUuid::createUuid();
  msg.timestamp = QDateTime::currentMSecsSinceEpoch();
  msg.topic = topic;
  msg.data = data;
  msg.from = Path();
  msg.type = Message::FakeSync;
  msg.feedback = topic + "-" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
  auto reply = dispatcher_->CreateReply(msg.feedback);

  emit sendRequest(msg);

  dispatcher_->WaitReply(reply, timeout);

  return reply;

  //    PODReply ret;
  //    if(QThread::currentThread() != this->thread())
  //    {
  //        bool r = QMetaObject::invokeMethod(this, k_mem_func, Qt::BlockingQueuedConnection
  //                                  , Q_RETURN_ARG(PODReply, ret)
  //                                  , Q_ARG(Message, msg)
  //                                  , Q_ARG(int, timeout));
  //        Q_ASSERT(r);
  //    }
  //    else
  //    {
  //        bool r = QMetaObject::invokeMethod(this, k_mem_func, Qt::DirectConnection
  //                                  , Q_RETURN_ARG(PODReply, ret)
  //                                  , Q_ARG(Message, msg)
  //                                  , Q_ARG(int, timeout));
  //        Q_ASSERT(r);
  //    }

  //    auto reply = ReplyPtr(dispatcher_->CreateReply(topic));
  //    reply->SetData(ret.data);
  //    if(ret.error_)
  //    {
  //        reply->SetError(InvokeError(ret.error_, ret.message_));
  //    }

  return reply;
}

ReplyPtr Object::Request(const int& topic, const QVariant& data, int timeout)
{
  return Request(QString::number(topic), data, timeout);
}

ReplyPtr Object::Request(const int& topic, const google::protobuf::Message& data, int timeout)
{
  return Request(topic, QByteArray::fromStdString(data.SerializeAsString()), timeout);
}

ReplyPtr Object::BlockRequest(const QString& topic, const QVariant& data, int timeout)
{
  QThread::msleep(1);

  Message msg;
  msg.uuid = QUuid::createUuid();
  msg.timestamp = QDateTime::currentMSecsSinceEpoch();
  msg.topic = topic;
  msg.data = data;
  msg.from = Path();
  msg.type = Message::Sync;
  msg.feedback = topic + "-" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");

  // qInfo()<<"begin invoke"<<topic<<"with feedback topic"<<msg.feedback;

  dispatcher_->TrackFeedback(msg.feedback);

  // if one blockrequest itself, will be deadlock
  auto promise = QSharedPointer<std::promise<Message>>(new std::promise<Message>);

  auto future = promise->get_future();

  emit sendBlockRequest(msg, promise.toWeakRef());

  auto reply = ReplyPtr(dispatcher_->CreateReply(topic));
  if (std::future_status::timeout == future.wait_for(std::chrono::milliseconds(timeout)))
  {
    qWarning() << "timeout!!!" << timeout << topic;
    dispatcher_->UntrackFeedback(topic);

    promise->set_value(msg);
    reply->SetError(InvokeError::TimeoutError());
  }

  reply->SetData(future.get().data);

  return reply;
}

ReplyPtr Object::BlockRequest(const int& topic, const QVariant& data, int timeout)
{
  return BlockRequest(QString::number(topic), data, timeout);
}

ReplyPtr Object::BlockRequest(const int& topic, const google::protobuf::Message& data, int timeout)
{
  return BlockRequest(topic, QByteArray::fromStdString(data.SerializeAsString()), timeout);
}

QVariant Object::Invoke(const int& topic, const QVariant& data)
{
  Message msg;
  msg.uuid = QUuid::createUuid();
  msg.timestamp = QDateTime::currentMSecsSinceEpoch();
  msg.topic = QString::number(topic);
  msg.data = data;
  msg.from = Path();
  msg.feedback.clear();
  msg.type = Message::DangerSync;

  return emit sendInvoke(msg);
}

QVariant Object::Invoke(const int& topic, const google::protobuf::Message& data)
{
  return Invoke(topic, QByteArray::fromStdString(data.SerializeAsString()));
}

ReplyPtr Object::MakeWish(const QString& topic)
{
  dispatcher_->TrackWish(topic);
  Subscribe(topic);

  auto reply = dispatcher_->CreateReply(topic);
  QObject::connect(reply.data(), &Reply::closed, this,
    [this, topic]
    {
      dispatcher_->UntrackWish(topic);
      Unsubscribe(topic);
    });

  return reply;
}

ReplyPtr Object::MakeWish(const int& topic)
{
  return MakeWish(QString::number(topic));
}

void Object::RegisterNotifyHandler(const QString& topic, std::function<NotifyHandler> handler)
{
  if (topic.isEmpty())
  {
    return;
  }

  dispatcher_->Bind(topic, handler);

  Subscribe(topic, false);
}

void Object::RegisterRequestHandler(const QString& topic, std::function<RequestHandler> handler)
{
  if (topic.isEmpty())
  {
    return;
  }

  dispatcher_->Bind(topic, handler);

  Subscribe(topic, true);
}

QDebug Object::Debug(const char* file, int line, const char* function) const
{
  return QMessageLogger(file, line, function, dispatcher_->ObjectAddress()).debug();
}

QDebug Object::Info(const char* file, int line, const char* function) const
{
  return QMessageLogger(file, line, function, dispatcher_->ObjectAddress()).info();
}

QDebug Object::Warning(const char* file, int line, const char* function) const
{
  return QMessageLogger(file, line, function, dispatcher_->ObjectAddress()).warning();
}

QDebug Object::Critical(const char* file, int line, const char* function) const
{
  return QMessageLogger(file, line, function, dispatcher_->ObjectAddress()).critical();
}

void Object::Assign(const Address& address, const ObjectFlag& flag)
{
  setProperty("ObjectPath", address);
  setProperty("ObjectFlag", int(flag));

  dispatcher_->SetObjectAddress(address);
}

void Object::Preset(const QStringList& topics, bool invokable)
{
  dispatcher_->UpdateDict(topics, invokable);
}

void Object::HandleNotify(const Message& msg)
{
  dispatcher_->HandleNotify(msg);
}

void Object::HandleRequest(const Message& msg)
{
  auto data = dispatcher_->HandleRequest(msg);

  Message reply;
  reply.uuid = msg.uuid;
  reply.timestamp = QDateTime::currentMSecsSinceEpoch();
  reply.topic = msg.feedback;
  reply.data = data;
  reply.from = Path();
  reply.feedback = msg.topic;
  reply.to = msg.from;

  emit sendPub(reply);
}

void Object::HandleBlockRequest(const Message& msg, QWeakPointer<MsgPromise> prom)
{
  // std::lock_guard<std::timed_mutex> guard(mutex_promli_);

  auto data = dispatcher_->HandleRequest(msg);

  //    qDebug() << QThread::currentThreadId();

  Message reply;
  reply.uuid = msg.uuid;
  reply.timestamp = QDateTime::currentMSecsSinceEpoch();
  reply.topic = msg.feedback;
  reply.data = data;
  reply.from = Path();
  reply.feedback = msg.topic;

  using namespace std::chrono_literals;
  // if(mutex_promli_.try_lock_for(600ms))
  //{
  if (prom.lock())
  {
    prom.lock()->set_value(reply);
  }
  //    mutex_promli_.unlock();
  //}
}

void Object::HandleFeedback(const Message& msg)
{

  dispatcher_->HandleFeedback(msg);
}

QVariant Object::HandleInvoke(const Message& msg)
{
  return dispatcher_->HandleRequest(msg);
}

void Object::Init() {}

void Object::Kill() {}

void Object::AboutToKill() {}

void Object::Run() {}

}
