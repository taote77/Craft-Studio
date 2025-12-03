#include "uzmq/message.h"

#include <boost/format.hpp>

namespace uzmq
{

#define TOPIC_KEY ("__Z_M_TOPIC__")
#define TIMESTAMP_KEY ("__Z_M_TIMESTAMP__")
#define FROM_KEY ("__Z_M_FROM__")
#define RECEIPT_KEY ("__Z_M_RECEIPT__")
#define ERROR_CODE_KEY ("__Z_M_ERRCODE__")
#define BODY_KEY ("__Z_M_BODY_DATA__")

struct Message::Context
{
    ZAny data;

    std::string topic;

    std::string from;

    time_t timestamp;

    std::string receipt;

    ErrorCode code;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& data;
        ar& topic;
        ar& from;
        ar& timestamp;
        ar& receipt;
        ar& code;
    }
};

Message::Message()
{
    mContext = std::make_shared<Context>();
}

Message::Message(const std::string& topic, const ZAny& obj, const std::string& objName)
{
    using namespace std::chrono;
    mContext = std::make_shared<Context>();
    mContext->topic = topic;
    mContext->data = obj;
    mContext->from = objName;
    auto now = std::chrono::steady_clock::now();
    mContext->timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    mContext->receipt = str(boost::format("%s-%s-%lld") % mContext->topic % mContext->from % mContext->timestamp);
    mContext->code = NO_ERROR;
}

Message::Message(const ErrorCode &code)
{
    using namespace std::chrono;
    mContext = std::make_shared<Context>();
    mContext->topic = "";
    mContext->from = "";
    auto now = std::chrono::steady_clock::now();
    mContext->timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    mContext->receipt = str(boost::format("%s-%s-%lld") % mContext->topic % mContext->from % mContext->timestamp);
    mContext->code = code;
    mContext->data = ZAny();
}

Message::Message(const Message &other)
{
    mContext = other.mContext;
}

Message& Message::operator=(const Message& other)
{
    mContext = other.mContext;
    return *this;
}

std::string Message::from() const
{
    return mContext->from;
}

std::string Message::topic() const
{
    return mContext->topic;
}

ZAny& Message::data() const
{
    return mContext->data;
}

time_t Message::timestamp() const
{
    return mContext->timestamp;
}

std::string Message::receipt() const
{
    return mContext->receipt;
}

std::string Message:: serialize() const
{
    try
    {
        std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << (*mContext);
        return oss.str();
    }
    catch (std::exception& e)
    {
        throw std::runtime_error(std::string("Failed to serialize message.Error:") + e.what());
    }
}

void Message::deserialize(const std::string &str)
{
    try
    {
        std::istringstream iss(str);
        boost::archive::text_iarchive ia(iss);
        ia >> (*mContext);
    }
    catch (std::exception& e)
    {
        throw std::runtime_error("Failed to deserialize message:" + serialize() + ",Error:" + e.what());
    }
}

void Message::setErrCode(ErrorCode code)
{
    mContext->code = code;
}

Message::ErrorCode Message::errCode()
{
    return mContext->code;
}

bool Message::isValid() const
{
    if (mContext->data.is_null())
    {
        return false;
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const Msg& msg)
{
    os << msg.serialize();
    return os;
}

std::ostream& operator<<(std::ostream& os, const Msg::ErrorCode &code)
{
    std::string message = "";
    switch (code) {
    case Msg::NO_ERROR:
        message = "NO_ERROR";
        break;
    case Msg::MSGQ_IS_NULLPTR:
        message = "MSGQ_IS_NULLPTR";
        break;
    case Msg::MSG_LOOP_RUNNING:
        message = "MSG_LOOP_RUNNING";
        break;
    case Msg::MSG_LOOP_INVALID:
        message = "MSG_LOOP_INVALID";
        break;
    case Msg::MSG_LOOP_EME_STOP:
        message = "MSG_LOOP_EME_STOP";
        break;
    case Msg::SOCKET_NULL_ERROR:
        message = "SOCKET_NULL_ERROR";
        break;
    case Msg::SEND_SIZE_ERROR:
        message = "SEND_SIZE_ERROR";
        break;
    case Msg::SEND_ERROR:
        message = "SEND_ERROR";
        break;
    case Msg::REPLY_TIMEOUT:
        message = "REPLY_TIMEOUT";
        break;
    case Msg::REPLY_NO_MATCH:
        message = "REPLY_NO_MATCH";
        break;
    case Msg::RECV_COUNT_ERROR:
        message = "RECV_COUNT_ERROR";
        break;
    case Msg::RECV_MSGS_ERROR:
        message = "RECV_MSGS_ERROR";
        break;
    case Msg::FUNC_IS_NULLPTR:
        message = "FUNC_IS_NULLPTR";
        break;
    case Msg::ROUTER_IS_EMPTY:
        message = "ROUTER_IS_EMPTY";
        break;
    case Msg::SERVER_IS_BUSY:
        message = "SERVER_IS_BUSY";
        break;
    default:
        message = "Invalid error code";
        break;
    }
    os << message;
    return os;
}

}
