#include "uzmq/env.h"
#include "uzmq/object.h"
#include "msgq.h"
#include "plugin.h"
#include "appinst.h"

namespace uzmq
{

class Object::Context
{
public:
    explicit Context()
    {
        auto router = AppInst::getRouter();
        mMsgQ = std::make_shared<MsgQ>(router);
    }

    virtual ~Context()
    {

    }

    const std::shared_ptr<MsgQ>& getMsgQ()
    {
        return mMsgQ;
    }


private:
    std::shared_ptr<MsgQ> mMsgQ;
};


Object::Object(): mContext(std::make_unique<Context>())
{

}

Object::~Object()
{

}

bool Object::exportObj(const std::shared_ptr<Object>& object, std::string name)
{
    if(object.get() != nullptr)
    {
        object->setObjName(name);
    }
    return Plugin::getInstance().add(object);
}


void Object::init()
{

}

void Object::uninit()
{

}

void Object::publish(const int& topic, const ZAny& data)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        msgQ.lock()->publish(std::to_string(topic), data);
    }
}

void Object::publish(const std::string &topic, const ZAny &data)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        msgQ.lock()->publish(topic, data);
    }
}

Msg Object::request(const int &topic, const ZAny &data, const int &timeout)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->request(std::to_string(topic), data, timeout);
    }
    return Msg::MSGQ_IS_NULLPTR;
}

Msg Object::request(const std::string &topic, const ZAny &data, const int &timeout)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->request(topic, data, timeout);
    }
    return Message::MSGQ_IS_NULLPTR;
}

Msg Object::invoke(const int &topic, const ZAny &data)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->invoke(std::to_string(topic), data);
    }
    return Message::MSGQ_IS_NULLPTR;
}

Msg Object::invoke(const std::string &topic, const ZAny &data)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->invoke(topic, data);
    }
    return Message::MSGQ_IS_NULLPTR;
}

void Object::bind(const int &topic, Msg::NoReplyFunc func, bool block)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->add(std::to_string(topic), func, block);
    }
}

void Object::bind(const std::string &topic, Message::NoReplyFunc func, bool block)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->add(topic, func, block);
    }
}

void Object::bind(const int &topic, Msg::ReplyFunc func, bool block)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->add(std::to_string(topic), func, block);
    }
}

void Object::bind(const std::string &topic, Message::ReplyFunc func, bool block)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->add(topic, func, block);
    }
}

void Object::setObjName(std::string name)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->setObjName(name);
    }
}

void Object::startUp()
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        msgQ.lock()->run(std::bind(&Object::init, this), std::bind(&Object::uninit, this));
    }
}

void Object::disconnect(const int &topic)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->remove(std::to_string(topic));
    }
}

void Object::disconnect(const std::string &topic)
{
    std::weak_ptr<MsgQ> msgQ = mContext->getMsgQ();
    if(!msgQ.expired())
    {
        return msgQ.lock()->remove(topic);
    }
}
}

