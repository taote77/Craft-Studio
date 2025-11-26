#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QMutexLocker>

#include "plugin.h"

namespace pdk {

class Router : public QObject
{
    Q_OBJECT
public:
    explicit Router(QObject *parent = nullptr);

    void Attacth(Plugin* plugin);

    void Detach(Plugin* plugin);
signals:

public slots:
    void OnSub(const Message& msg);

    void OnUnsub(const Message& msg);

    void OnPub(const Message& msg);

    void OnRequest(const Message& msg);

    void OnBlockRequest(const Message& msg, QWeakPointer<MsgPromise> prom);

    QVariant OnInvoke(const Message& msg);

protected:
    void AddSubscriber(const Address& address,const QString& topic);

    void AddInvoker(const Address& address,const QString& topic);

    void RemoveSubscriber(const Address& address,const QString& topic=QString());

    void RemoveInvoker(const QString& topic);

private:
    QMap<QString,QWeakPointer<Object>> objects_;

    QMap<QString,QList<Address>> subscribers_;

    QMap<QString,Address> invokers_;

    QMutex mutex_;
};

}


#endif // ROUTER_H
