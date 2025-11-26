#include "dispatcher.h"


namespace pdk
{


Dispatcher::Dispatcher(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<Message>("Message");
}

void Dispatcher::Bind(const QString &topic, std::function<NotifyHandler> handler)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    notify_handlers_[topic] = handler;

    TrackWish(topic);
}

void Dispatcher::Bind(const QString &topic, std::function<RequestHandler> handler)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    request_handlers_[topic] = handler;
}

void Dispatcher::TrackFeedback(const QString &topic)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    feedbacks_.insert(topic);
}

void Dispatcher::UntrackFeedback(const QString &topic)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    feedbacks_.remove(topic);
}

void Dispatcher::TrackWish(const QString &topic)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    wishes_.insert(topic);
}

void Dispatcher::UntrackWish(const QString &topic)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    wishes_.remove(topic);
}

void Dispatcher::Unbind(const QString &topic, bool invokable)
{
    if(invokable)
    {
        request_handlers_.remove(topic);
    }
    else
    {
        notify_handlers_.remove(topic);
    }
}

QVariant Dispatcher::HandleRequest(const Message &msg)
{
    if(request_handlers_.contains(msg.topic))
    {
        return request_handlers_[msg.topic](msg.data);
    }

    return QVariant();
}

void Dispatcher::HandleNotify(const Message &msg)
{
    if(!wishes_.contains(msg.topic)&&!notify_handlers_.contains(msg.topic))
    {
        qWarning()<<"topic"<<msg.topic<<"not found!";
        return;
    }

    if(wishes_.contains(msg.topic))
    {
        emit recv(msg.topic,msg.data);
    }

    if(notify_handlers_.contains(msg.topic))
    {
        notify_handlers_[msg.topic](msg.data);
    }
}

void Dispatcher::HandleFeedback(const Message &msg)
{
    if(feedbacks_.contains(msg.topic))
    {
        emit recv(msg.topic,msg.data);
    }
    else
    {
        qWarning()<<"request"<<msg.topic<<"not found,maybe it's timeout!";
    }
}

void Dispatcher::UpdateDict(const QStringList& topics,bool invokable)
{
    if(invokable)
    {
        invoketion_dict_ = topics;
    }
    else
    {
        subscription_dict_ = topics;
    }
}

QString Dispatcher::Translate(const int& index,bool invokable)
{
    if(invokable)
    {
        return invoketion_dict_.value(index,QString::number(index));
    }

    return subscription_dict_.value(index,QString::number(index));
}

void Dispatcher::SetObjectAddress(const QString &address)
{
    address_ = address.toStdString();
}

const char *Dispatcher::ObjectAddress() const
{
    return address_.data();
}

ReplyPtr Dispatcher::WaitForFeedback(const QString &topic,int timeout)
{
    //TrackFeedback(topic);

    auto reply = CreateReply(topic);

    reply->Wait(timeout);

    UntrackFeedback(topic);

    return ReplyPtr(reply);
}

ReplyPtr Dispatcher::CreateReply(const QString &topic)
{
    TrackFeedback(topic);

    auto reply = ReplyPtr::create(topic);

    QObject::connect(this,&Dispatcher::recv,reply.data(),&Reply::Recv);

    return reply;
}

void Dispatcher::WaitReply(ReplyPtr reply,int timeout)
{
    Q_ASSERT(!reply.isNull());

    reply->Wait(timeout);

    UntrackFeedback(reply->topic_);
}


















}

