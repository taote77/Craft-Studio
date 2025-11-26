#ifndef ULTRABUS_OBJECT_H
#define ULTRABUS_OBJECT_H


#include <QObject>
#include <QDebug>
#include <QSharedPointer>
#include <QLoggingCategory>

#include <pdk/global.h>
#include <pdk/message.h>
#include <pdk/reply.h>
#include <map>
#include <chrono>

namespace pdk {

class Dispatcher;

class PDK_API Object : public QObject
{
    Q_OBJECT
public:
    Object();

    virtual ~Object();

    enum class ObjectFlag
    {
        Gui,
        Serviced,
        Misc,
    };

public:
    Address Path() const;

    ObjectFlag Flag() const;


signals:
    void sendSub(const pdk::Message& msg);

    void sendUnsub(const pdk::Message& msg);

    void sendPub(const pdk::Message& msg);

    void sendRequest(const pdk::Message& msg);

    void sendBlockRequest(const pdk::Message& msg, QWeakPointer<MsgPromise> prom);

    QVariant sendInvoke(const pdk::Message& msg);

public slots:
    void HandleNotify(const pdk::Message& msg);

    void HandleRequest(const pdk::Message& msg);

    void HandleBlockRequest(const pdk::Message& msg, QWeakPointer<MsgPromise> prom);

    void HandleFeedback(const pdk::Message& msg);

    QVariant HandleInvoke(const pdk::Message& msg);

    virtual void Init();

    virtual void Kill();

    virtual void AboutToKill();

    virtual void Run();

private:
    friend class Plugin;

    void Assign(const Address& address,const ObjectFlag& flag);

    void Preset(const QStringList& topics,bool invokable);
public slots:
    void Subscribe(const QString& topic, bool invokable = false);

    void Subscribe(const int& topic, bool invokable = false);

    void Unsubscribe(const QString& topic,bool invokable = false);

    void Unsubscribe(const int& topic,bool invokable = false);

    void Publish(const QString& topic,const QVariant& data=QVariant());

    void Publish(const int& topic,const QVariant& data=QVariant());

    void Publish(const int& topic,const google::protobuf::Message& data);

protected slots:
    PODReply RequestImpl(Message msg, int timeout=20000);

public:
    ReplyPtr Request(const QString& topic,const QVariant& data=QVariant(),int timeout=20000);

    ReplyPtr Request(const int& topic,const QVariant& data=QVariant(),int timeout=20000);

    ReplyPtr Request(const int& topic,const google::protobuf::Message& data,int timeout=20000);

    ReplyPtr BlockRequest(const QString& topic,const QVariant& data=QVariant(),int timeout=20000);

    ReplyPtr BlockRequest(const int& topic,const QVariant& data=QVariant(),int timeout=20000);

    ReplyPtr BlockRequest(const int& topic,const google::protobuf::Message& data,int timeout=20000);

    QVariant Invoke(const int& topic,const QVariant& data=QVariant());

    QVariant Invoke(const int& topic,const google::protobuf::Message& data);

    ReplyPtr MakeWish(const QString& topic);

    ReplyPtr MakeWish(const int& topic);

protected:
    void RegisterNotifyHandler(const QString& topic,std::function<NotifyHandler> handler);

    void RegisterRequestHandler(const QString& topic,std::function<RequestHandler> handler);

    template<typename O>
    void Bind(const int& topic, void (O::*fn)(const QVariant&),O* obj)
    {
        RegisterNotifyHandler(QString::number(topic), std::bind(fn,obj, std::placeholders::_1));
    }

    template<typename O>
    void Bind(const int& topic,  QVariant (O::*fn)(const QVariant&),O* obj)
    {
        RegisterRequestHandler(QString::number(topic), std::bind(fn,obj, std::placeholders::_1));
    }

protected:
    QDebug Debug(const char* file=nullptr,int line=0,const char* function=nullptr) const;

    QDebug Info(const char* file=nullptr,int line=0,const char* function=nullptr) const;

    QDebug Warning(const char* file=nullptr,int line=0,const char* function=nullptr) const;

    QDebug Critical(const char* file=nullptr,int line=0,const char* function=nullptr) const;

private:
    Dispatcher* dispatcher_;

    //std::timed_mutex mutex_promli_;

};

#define PDK_OBJECT_INIT void Init() override
#define PDK_BIND(topic,method) Bind(topic,&std::remove_pointer<decltype (this)>::type::method,this)
}

#include <pdk/register.h>

#endif // ULTRABUS_OBJECT_H
