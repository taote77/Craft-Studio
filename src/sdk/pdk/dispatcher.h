#ifndef ULTRABUS_DISPATCHER_H
#define ULTRABUS_DISPATCHER_H

#include <QObject>
#include <QMutex>
#include <shared_mutex>
#include <mutex>
#include <QSemaphore>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <QSet>

#include "message.h"
#include "reply.h"

namespace pdk {

class Dispatcher : public QObject
{
    Q_OBJECT
public:
    explicit Dispatcher(QObject *parent = nullptr);

    void Bind(const QString& topic,std::function<NotifyHandler> handler);

    void Bind(const QString& topic,std::function<RequestHandler> handler);

    void Unbind(const QString& topic,bool invokable=false);

    void TrackFeedback(const QString& topic);

    void UntrackFeedback(const QString& topic);

    void TrackWish(const QString& topic);

    void UntrackWish(const QString& topic);

    QVariant HandleRequest(const Message& msg);

    void HandleNotify(const Message& msg);

    void HandleFeedback(const Message& msg);

    void UpdateDict(const QStringList& topics,bool invokable = false);

    QString Translate(const int& index,bool invokable = false);

    void SetObjectAddress(const QString& address);

    const char* ObjectAddress() const;

    ReplyPtr WaitForFeedback(const QString& topic,int timeout);

    ReplyPtr CreateReply(const QString& topic);

    void WaitReply(ReplyPtr reply, int timeout);

signals:
    void recv(const QString& topic,const QVariant& data);

private:
    QMap<QString,std::function<NotifyHandler>> notify_handlers_;

    QMap<QString,std::function<RequestHandler>> request_handlers_;

    QSet<QString> feedbacks_;

    QSet<QString> wishes_;

    QStringList invoketion_dict_;

    QStringList subscription_dict_;

    std::recursive_mutex mutex_;

    std::string address_;
};



}


#endif // ULTRABUS_DISPATCHER_H
