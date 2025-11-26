#include "message_bus.h"

using namespace pdk;

MessageBus::MessageBus(QObject *parent) : QObject(parent)
{

}

MessageBus *MessageBus::Instance()
{
    static MessageBus instance(nullptr);

    return &instance;
}

void MessageBus::Post(const QString &topic, const Address &addr, const QVariant &data)
{
    Message msg;
    msg.uuid = QUuid::createUuid();
    msg.timestamp = QDateTime::currentMSecsSinceEpoch();
    msg.topic = topic;
    msg.data = data;
    msg.from = addr;
    msg.type = Message::Async;

    Q_EMIT sigPub(msg);
}

void MessageBus::Post(const int &topic, const pdk::Address&addr, const QVariant &data)
{
    Post(QString::number(topic), addr, data);
}

void MessageBus::Post(const int& topic, const Address &addr, const google::protobuf::Message& data)
{
    Post(topic, addr, QByteArray::fromStdString(data.SerializeAsString()));
}
