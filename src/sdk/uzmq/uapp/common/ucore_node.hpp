#pragma once

#include "global/env.hpp"
#include "global/log.hpp"
#include "global/message.hpp"
#include "global/object.hpp"

#include "ucore_api.hpp"


namespace heygears {

namespace proto_uc {

// 用于简化websocket构造和答复流程,注意只能执行一次析构
class WebReplyMsg 
{
private:
    heygears::proto_uc::WebMessage mWmsg;
    uzmq::Object* mObjPtr { nullptr };
public:
    WebReplyMsg(const uzmq::Message& umsg, uzmq::Object* object)
    {
        try
        {
            mWmsg = umsg.data().valueTo<heygears::proto_uc::WebMessage>();
            mObjPtr = object;
        }
        catch(const std::exception& e)
        {
            mWmsg.error = heygears::UcoreErrorCode::UCORE_ERROR_DATA_CONVER;
            mWmsg.reason = e.what();
        }
    }
    WebReplyMsg(const uzmq::Message& umsg, uzmq::Object& object)
        : WebReplyMsg(umsg, &object) {}

    WebReplyMsg(const uzmq::Message& umsg, const std::shared_ptr<uzmq::Object>& object)
        : WebReplyMsg(umsg, object.get()) {}

    WebReplyMsg(const WebReplyMsg&) = delete;
    WebReplyMsg& operator=(const WebReplyMsg&) = delete;

    WebReplyMsg(WebReplyMsg&& other) noexcept
        : mWmsg(std::move(other.mWmsg)), mObjPtr(other.mObjPtr)
    {
        other.mObjPtr = nullptr;
    }

    WebReplyMsg& operator=(WebReplyMsg&& other) noexcept
    {
        if (this != &other)
        {
            mWmsg = std::move(other.mWmsg);
            mObjPtr = other.mObjPtr;
            other.mObjPtr = nullptr;
        }
        return *this;
    }
    ~WebReplyMsg()
    {
        if (!mWmsg.topic.empty() && mObjPtr)
        {
            mObjPtr->publish(heygears::proto_px::GW_WS_REPLY,
                uzmq::ZAny::valueFrom<heygears::proto_uc::WebMessage>(mWmsg));
        }
    }
    boost::json::value& data(std::string key) 
    { 
        return mWmsg.data(key); 
    }

    template <typename T>
    T get(const std::string &key, const T &default_v) 
    { 
        return mWmsg.get(key, default_v); 
    }

    bool contains(std::string key)
    {
        return mWmsg.contains(key);
    }

    void update(const boost::json::object &data)
    {
        mWmsg.update(data);
    }
};

    
} // namespace proto_uc

} // namespace heygears

