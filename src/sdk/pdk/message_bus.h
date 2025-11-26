#ifndef PDK_MESSAGE_BUS_H
#define PDK_MESSAGE_BUS_H

#include <pdk/message.h>
#include <google/protobuf/message.h>
#include <QObject>
#include <mutex>

class MessageBus : public QObject
{
    Q_OBJECT
private:
    explicit MessageBus(QObject *parent = nullptr);

public:
    static MessageBus *Instance();

    void Post(const QString& topic, const pdk::Address&addr, const QVariant& data=QVariant());

    void Post(const int& topic, const pdk::Address&addr, const QVariant& data=QVariant());

    void Post(const int& topic, const pdk::Address&addr, const google::protobuf::Message& data);

signals:
    void sigPub(const pdk::Message &msg);

};

#endif // PDK_MESSAGE_BUS_H
