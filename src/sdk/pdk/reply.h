#ifndef REPLY_H
#define REPLY_H

#include <pdk/global.h>
#include <QSharedPointer>
#include <QVariant>
#include <QObject>
#include <QUuid>
#include <QEventLoop>
#include <QTimer>
#include <google/protobuf/message.h>

namespace pdk {

class PDK_API InvokeError:public std::exception{
public:
    InvokeError(int error = 0,const QString& msg=QString());

    InvokeError(const InvokeError& other);

    InvokeError& operator=(const InvokeError& other);

    operator QVariant() const;

    operator bool() const;

    int Code() const;

    QString Message() const;

    friend QDebug operator<<(QDebug out, const InvokeError& error);

    static const InvokeError ProtobufError();

    static const InvokeError TimeoutError();

    static const InvokeError TopicError();

    static const InvokeError TypeError();

    static const InvokeError JsonError();

    void Reset();

    const char* what() const throw();

private:
    int error_;

    std::string message_;
};


class PDK_API Reply:public QObject
{
    Q_OBJECT
public:
    Reply(const QString& topic);

    ~Reply();

    bool Ok() const;

    InvokeError Error() const;

    QVariant Data() const;

    void SetError(const InvokeError& error);

    void SetData(const QVariant& data);

    bool ToProtobuf(google::protobuf::Message* msg);

    template<class T>
    bool ToValue(T& value)
    {
        if(!Ok())
        {
            return false;
        }

        if(!data_.canConvert<T>())
        {
             SetError(InvokeError::TypeError());
             return false;
        }

        value = data_.value<T>();

        return true;
    }

public slots:
    void Wait(int timeout);

private slots:

    void Recv(const QString& topic,const QVariant& data);

    void Timeout();

signals:
    void wakeup(const QString& uuid,const QVariant& data,QPrivateSignal);

    void closed();

private:
    QVariant data_;

    const QString topic_;

    QSharedPointer<InvokeError> error_;

    QEventLoop* loop_;

    QTimer* timer_;

    friend class Dispatcher;
};


struct PODReply {
    QVariant data;
    int error_;
    QString message_;
};


using ReplyPtr = QSharedPointer<Reply>;


}

Q_DECLARE_METATYPE(pdk::InvokeError)
Q_DECLARE_METATYPE(pdk::PODReply)

#endif // REPLY_H
