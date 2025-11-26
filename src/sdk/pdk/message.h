#ifndef MESSAGE_H
#define MESSAGE_H

#include <QVariant>
#include <QMetaType>
#include <QDateTime>
#include <QUuid>

#include <memory>
#include <future>

#include <pdk/address.h>

namespace pdk {

struct Message
{
    QUuid uuid;

    Address from;

    Address to;

    QString topic;

    QString feedback;

    QVariant data;

    quint64 timestamp;

    enum Type{
        Async,      // Publish:
        FakeSync,   // Request:         eventloop can still work
        Sync,       // BlockRequest:    thread safe
        DangerSync  // Invoke:          not thread safe
    };
    Type type = Message::Async;

};

using NotifyHandler = void(const QVariant&);

using RequestHandler = QVariant(const QVariant&);

using MsgPromise = std::promise<Message>;

}

Q_DECLARE_METATYPE(pdk::Message)

#endif // MESSAGE_H
