#ifndef UZMQ_MESSAGE_H
#define UZMQ_MESSAGE_H

#include <iostream>
#include <functional>
#include "zany.h"

namespace uzmq
{

class Message
{
public:

    enum ErrorCode
    {
        NO_ERROR = 0,
        MSGQ_IS_NULLPTR,
        MSG_LOOP_RUNNING,
        MSG_LOOP_INVALID,
        MSG_LOOP_EME_STOP,
        SOCKET_NULL_ERROR,
        SEND_SIZE_ERROR,
        SEND_ERROR,
        REPLY_TIMEOUT,
        REPLY_NO_MATCH,
        RECV_COUNT_ERROR,
        RECV_MSGS_ERROR,
        FUNC_IS_NULLPTR,
        ROUTER_IS_EMPTY,
        SERVER_IS_BUSY,
    };

    using ReplyFunc = std::function<ZAny (const Message&)>;
    using NoReplyFunc = std::function<void(const Message&)>;

    Message();

    Message(const std::string &topic, const ZAny &obj, const std::string &objName);

    Message(const ErrorCode& code);

    Message(const Message& other);

    Message &operator=(const Message &other);

    std::string from() const;

    std::string topic() const;

    ZAny& data() const;

    time_t timestamp() const;

    std::string receipt() const;

    std::string serialize() const;

    void deserialize(const std::string& str);

    void setErrCode(ErrorCode code);

    ErrorCode errCode();

    bool isValid() const;

private:
    struct Context;
    std::shared_ptr<Context> mContext;
};

using Msg = Message;

std::ostream &operator<<(std::ostream &os, const Msg &msg);

std::ostream &operator<<(std::ostream &os, const Msg::ErrorCode &code);

}


#endif // MESSAGE_H
