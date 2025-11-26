#include "reply.h"
#include <QMetaType>
#include <QDebug>


namespace pdk {


Reply::Reply(const QString& topic)
    :topic_(topic)
    ,loop_(new QEventLoop(this))
    ,timer_(new QTimer(this))
{
    QObject::connect(timer_,&QTimer::timeout,this,&Reply::Timeout);
}

Reply::~Reply()
{
    emit closed();
}

bool Reply::Ok() const
{
    return error_.isNull();
}

InvokeError Reply::Error() const
{
    if(error_.isNull())
    {
        return InvokeError();
    }

    return {error_->Code(),error_->Message() };
}


QVariant Reply::Data() const
{
    return data_;
}

void Reply::SetError(const InvokeError &error)
{
    error_.reset(new InvokeError(error));
}


void Reply::SetData(const QVariant &data)
{
    data_ = data;
}

bool Reply::ToProtobuf(google::protobuf::Message *msg)
{
    if(!Ok())
    {
        return false;
    }

    if(msg->ParseFromString(data_.toByteArray().toStdString()))
    {
        return true;
    }

    SetError(InvokeError::ProtobufError());

    return false;
}

void Reply::Wait(int timeout)
{
    if(timer_->isActive())
    {
        return;
    }

    timer_->start(timeout);

    loop_->exec();
}

void Reply::Recv(const QString &topic, const QVariant &data)
{
    if(topic==topic_&&loop_->isRunning())
    {
        timer_->stop();
        if(data.canConvert<InvokeError>())
        {
            SetError(data.value<InvokeError>());
        }
        else
        {
            SetData(data);
        }

        loop_->quit();
    }
}

void Reply::Timeout()
{
    if(loop_->isRunning())
    {
        SetError(InvokeError::TimeoutError());
        loop_->quit();
    }
}


InvokeError::InvokeError(int error, const QString &msg)
    :error_(error),message_(msg.toStdString())
{

}

InvokeError::InvokeError(const InvokeError &other)
    :error_(other.error_),message_(other.message_)
{

}

InvokeError &InvokeError::operator=(const InvokeError &other)
{
    error_ = other.error_;
    message_ = other.message_;
    return *this;
}

InvokeError::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

InvokeError::operator bool() const
{
    return error_!=0;
}

int InvokeError::Code() const
{
    return error_;
}

QString InvokeError::Message() const
{
    return message_.c_str();
}

const InvokeError InvokeError::ProtobufError()
{
    return {998,"protobuf parser error"};
}

const InvokeError InvokeError::TimeoutError()
{
    return {999,"timeout error"};
}

const InvokeError InvokeError::TopicError()
{
    return {997,"topic not found"};
}

const InvokeError InvokeError::TypeError()
{
    return {996,"data type error"};
}

const InvokeError InvokeError::JsonError()
{
    return {995,"json error"};
}

void InvokeError::Reset()
{
    error_ = 0;
    message_.clear();
}

const char *InvokeError::what() const throw()
{
    return message_.c_str();
}

QDebug operator<<(QDebug out, const InvokeError& error)
{
    out.nospace()<<"InvokeError("<<error.Code()<<","<<error.Message()<<")";

    return out;
}

}
