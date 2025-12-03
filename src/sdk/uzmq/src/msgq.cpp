#include "msgq.h"
#include "uzmq/log.h"

namespace uzmq
{
MsgQ::MsgQ(std::shared_ptr<Router> router)
    : mRouter(router)
{
    if(!mRouter.expired())
    {
        mPubAddr = mRouter.lock()->getPubAddr();
        mSubAddr = mRouter.lock()->getSubAddr();
        mComType = mRouter.lock()->getComType();
        mCtx = mRouter.lock()->getZmqContext();

        mPub = std::make_shared<zmq::socket_t>(*mCtx.lock(), zmq::socket_type::pub);
        mPub->connect(mSubAddr);
        mPub->set(zmq::sockopt::rcvhwm, mHightWaterLevel);
        mPub->set(zmq::sockopt::sndhwm, mHightWaterLevel);
    }
    mHandler.clear();
    mReplyMsgs.cleanup();
}

MsgQ::~MsgQ()
{
    close();
}

void MsgQ::setObjName(std::string name)
{
    mObjName = name;
}

int MsgQ::run(std::function<void (void)> init, std::function<void (void)> uninit)
{
    if(mMsgLoopCtrl.valid())
    {
        return Msg::MSG_LOOP_RUNNING;
    }

    mMsgLoopCtrl = std::async(std::launch::async, &MsgQ::msgLoop, this, init, uninit);

    return Msg::NO_ERROR;
}

int MsgQ::wait()
{
    if(false == mMsgLoopCtrl.valid())
    {
        return Msg::MSG_LOOP_INVALID;
    }
    return mMsgLoopCtrl.get();
}

int MsgQ::close()
{
    mReplyMsgs.cleanup();
    mStartFlag.store(false);
    auto ret = wait();
    mHandler.clear();
    mPub->close();
    mPub.reset();

    return ret;
}


void MsgQ::publish(const std::string &topic, const ZAny &data)
{
    std::unique_lock<std::mutex> lock(mPubMutex);
    if(mPub.get() != nullptr)
    {
        Msg msg(topic, data, mObjName);
        if(!socketSend(mPub, msg))
        {
            LOG_ERROR()  << __FUNCTION__ << "scoket send is error";
        }
    }
    else
    {
        LOG_ERROR()  << __FUNCTION__ << "publish scoket is nullptr";
    }
}

Msg MsgQ::request(const std::string &topic, const ZAny &data, int timeout)
{
    auto socket = std::make_shared<zmq::socket_t>(*mCtx.lock(), zmq::socket_type::req);
    socket->set(zmq::sockopt::rcvtimeo, timeout);
    Msg msg(topic, data, mObjName);
    auto reply = sendWaitRet(socket, msg, timeout);
    return reply;
}

Msg MsgQ::invoke(const std::string &topic, const ZAny &data)
{
    if(!mRouter.expired())
    {
        auto func = mRouter.lock()->directlyFind(topic);
        if(func)
        {
            Msg msg(topic, data, mObjName);
            auto retData = func(msg);
            Msg reply(topic, retData, mObjName);
            return reply;
        }
        else
        {
            return Msg::FUNC_IS_NULLPTR;
        }
    }
    return Msg::ROUTER_IS_EMPTY;
}

void MsgQ::add(const std::string &topic, Msg::NoReplyFunc func, bool block)
{
    std::unique_lock<std::mutex> lock(mHandlemutex);
    auto iter = mHandler.find(topic);
    if(iter == mHandler.end())
    {
        auto sub = std::make_shared<zmq::socket_t>(*(mCtx.lock()), zmq::socket_type::sub);
        sub->set(zmq::sockopt::subscribe, topic);
        sub->set(zmq::sockopt::rcvhwm, mHightWaterLevel);
        sub->set(zmq::sockopt::sndhwm, mHightWaterLevel);
        sub->set(zmq::sockopt::rcvtimeo, 100);
        sub->connect(mPubAddr);
        std::shared_ptr<boost::asio::thread_pool> pool = nullptr;
        if(false == block)
        {
            pool = std::make_shared<boost::asio::thread_pool>(1);
        }
        mHandler.emplace(topic, HandleMsgType{sub, func, nullptr, pool, block, false});
        mPoller.add(*sub, zmq::event_flags::pollin);
    }
}

void MsgQ::add(const std::string &topic, Msg::ReplyFunc func, bool block)
{
    std::unique_lock<std::mutex> lock(mHandlemutex);
    auto iter = mHandler.find(topic);
    if(iter == mHandler.end())
    {
        auto rep = std::make_shared<zmq::socket_t>(*(mCtx.lock()), zmq::socket_type::rep);
        rep->set(zmq::sockopt::rcvtimeo, 100);
        rep->bind(mComType + topic);
        std::shared_ptr<boost::asio::thread_pool> pool = nullptr;
        if(false == block)
        {
            pool = std::make_shared<boost::asio::thread_pool>(1);
        }
        mHandler.emplace(topic, HandleMsgType{rep, nullptr, func, pool, block, true});
        mPoller.add(*rep, zmq::event_flags::pollin);
    }
    lock.unlock();

    if(!mRouter.expired())
    {
        mRouter.lock()->directlyAdd(topic, func);
    }
}

void MsgQ::remove(const std::string &topic)
{
    std::unique_lock<std::mutex> lock(mHandlemutex);
    auto iter = mHandler.find(topic);
    if(iter != mHandler.end())
    {
        try
        {
            if(!iter->second.block)
            {
                iter->second.thPool->join();
            }

            if(!iter->second.replyFlag)
            {
                iter->second.socket->disconnect(mPubAddr);
            }
            else
            {
                iter->second.socket->unbind(mComType + topic);
            }
            iter->second.socket->close();
            mPoller.remove(*iter->second.socket);

            mHandler.erase(iter);
        }
        catch (zmq::error_t& e)
        {
            LOG_ERROR()  << __FUNCTION__ << ":catch error:" << e.what();
        }
        catch(const std::exception& e)
        {
            LOG_ERROR()  << __FUNCTION__ << ":catch error:" << e.what();
        }
    }
    lock.unlock();

    if(!mRouter.expired())
    {
        mRouter.lock()->directlyRemove(topic);
    }
}

int MsgQ::msgLoop(std::function<void(void)> init, std::function<void(void)> uninit)
{
    if(init)
    {
        init();
    }
    mStartFlag.store(true);

    std::unordered_map<std::string, std::string> msgReceipt;
    msgReceipt.clear();

    int ret = Msg::NO_ERROR;
    int printCount = 0;
    while(mStartFlag)
    {
        if(printCount++ > 20000)
        {
            LOG_DEBUG_BRIEF  << __FUNCTION__  << mObjName << " is alive";
            printCount = 0;
        }
        
        if(mHandler.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        else
        {
            Msg msg;
            while(mReplyMsgs.pop(msg))
            {
                auto iter = mHandler.find(msg.topic());
                if(iter != mHandler.end())
                {
                    socketSend(mHandler.at(msg.topic()).socket, msg);
                }
            }
            try
            {
                std::vector<zmq::poller_event<>> events;
                events.resize(mHandler.size());
                const std::chrono::milliseconds timeout(10);
                auto count = mPoller.wait_all(events, timeout);

                for (int num = 0; num < count; num++)
                {
                    if(num >= events.size())
                    {
                        break;
                    }
                    auto event = events.at(num);
                    if(static_cast<int>(event.events) & static_cast<int>(zmq::event_flags::pollin))
                    {
                        std::vector<zmq::message_t> recvMsgs;
                        auto result = zmq::recv_multipart(event.socket, std::back_inserter(recvMsgs));
                        if(result.has_value() && recvMsgs.size() == 2)
                        {
                            Msg rMsg;
                            rMsg.deserialize(recvMsgs.at(1).to_string());
                            if(checkMsgValid(recvMsgs.at(0).to_string(), rMsg, msgReceipt))
                            {
                                handleMsg(rMsg);
                            }
                        }
                    }
                }
            }
            catch (zmq::error_t& e)
            {
                LOG_ERROR()  << __FUNCTION__ << ":catch error:" << e.what();
                ret = Msg::MSG_LOOP_EME_STOP;
            }
        }
    }

    LOG_ERROR()  << __FUNCTION__ << "loop is exit:" << mObjName;

    if(uninit)
    {
        uninit();
    }

    return ret;
}

Msg::ErrorCode MsgQ::recvTimeout(const std::shared_ptr<zmq::socket_t> &socket, std::vector<zmq::message_t>& recvMsgs, int timeout)
{
    zmq::poller_t<> poller;
    std::vector<zmq::poller_event<>> events;
    events.resize(1);
    const std::chrono::milliseconds time(timeout);
    try
    {
        poller.add(*socket, zmq::event_flags::pollin);
        auto count = poller.wait_all(events, time);
        if (!count)
        {
            poller.remove(*socket);
            return Msg::REPLY_TIMEOUT;
        }
        else
        {
            for (int num = 0; num < count; num++)
            {
                if(num >= events.size())
                {
                    break;
                }
                auto event = events.at(num);
                if(static_cast<int>(event.events) & static_cast<int>(zmq::event_flags::pollin))
                {
                    auto result = zmq::recv_multipart(*socket, std::back_inserter(recvMsgs));
                    if(!result.has_value() || recvMsgs.size() != 2)
                    {
                        poller.remove(*socket);
                        LOG_ERROR()  << __FUNCTION__ << "recv msg is error.";
                        return Msg::RECV_COUNT_ERROR;
                    }
                    else
                    {
                        poller.remove(*socket);
                        return Msg::NO_ERROR;
                    }
                }
            }
        }
    }
    catch (zmq::error_t& e)
    {
        LOG_ERROR()  << __FUNCTION__ << ":catch error:" << e.what();
    }

    return Msg::RECV_MSGS_ERROR;
}

Msg MsgQ::sendWaitRet(const std::shared_ptr<zmq::socket_t> &socket, const Msg &msg, int timeout)
{
    if(socket)
    {
        try
        {
            socket->connect(mComType + msg.topic());
            if(!socketSend(socket, msg))
            {
                return Msg::SEND_SIZE_ERROR;
            }
        }
        catch (zmq::error_t& e)
        {
            LOG_ERROR()  << __FUNCTION__ << ":catch error:" << e.what();
            return Msg::SEND_ERROR;
        }

        std::vector<zmq::message_t> recvMsgs;
        auto ret = recvTimeout(socket, recvMsgs, timeout);
        socket->disconnect(mComType + msg.topic());
        if(ret != Msg::NO_ERROR)
        {
            return ret;
        }

        if(msg.topic() != recvMsgs.at(0).to_string())
        {
            return Msg::REPLY_NO_MATCH;
        }

        Msg reply;
        reply.deserialize(recvMsgs.at(1).to_string());
        return reply;
    }
    return Msg::SOCKET_NULL_ERROR;
}

bool MsgQ::sendReady(const std::shared_ptr<zmq::socket_t> &socket, int timeout)
{
    zmq::poller_t<> poller;
    std::vector<zmq::poller_event<>> events;
    events.resize(1);
    const std::chrono::milliseconds time(timeout);
    try
    {
        poller.add(*socket, zmq::event_flags::pollout);
        auto count = poller.wait_all(events, time);
        if (!count)
        {
            poller.remove(*socket);
            return false;
        }
        else
        {
            for (int num = 0; num < count; num++)
            {
                if(num >= events.size())
                {
                    break;
                }
                auto event = events.at(num);
                if(static_cast<int>(event.events) & static_cast<int>(zmq::event_flags::pollout))
                {
                    poller.remove(*socket);
                    return true;
                }
            }
        }
    }
    catch (zmq::error_t& e)
    {
        LOG_ERROR()  << __FUNCTION__ << ":catch error:" << e.what();
    }

    return false;
}

bool MsgQ::socketSend(const std::shared_ptr<zmq::socket_t> &socket, const Msg& msg)
{
    if(socket && sendReady(socket))
    {
        zmq::message_t dataT(msg.topic());
        zmq::message_t dataM(msg.serialize());
        socket->send(dataT, zmq::send_flags::sndmore);
        auto ret = socket->send(dataM, zmq::send_flags::none);
        return (ret.value() == msg.serialize().size());
    }
    LOG_ERROR()  << __FUNCTION__ << ":send socket is not ready or socket is nullptr.";
    return false;
}

void MsgQ::handleMsg(const Msg& msg)
{
    auto iter = mHandler.find(msg.topic());
    if(iter != mHandler.end())
    {
        HandleMsgType handler = iter->second;

        if((!handler.block) && handler.thPool)
        {
            boost::asio::post(*handler.thPool, std::bind(&MsgQ::runFuncInThread, this, msg, handler));
        }
        else
        {
            auto reply = runFunc(msg, handler);
            if(handler.replyFlag)
            {
                if(reply.isValid())
                {
                    socketSend(handler.socket, reply);
                }
            }
        }
    }
}

void MsgQ::runFuncInThread(Msg msg, HandleMsgType handler)
{
    auto reply = runFunc(msg, handler);
    if(handler.replyFlag)
    {
        if(reply.isValid())
        {
            mReplyMsgs.push(reply);
        }
    }
}

Msg MsgQ::runFunc(const Msg& msg, HandleMsgType handler)
{
    if(handler.replyFlag)
    {
        if(handler.RelpyFunc == nullptr)
        {
            return Msg();
        }
        auto data = handler.RelpyFunc(msg);
        return Msg(msg.topic(), data, mObjName);
    }
    else
    {
        if(handler.noRelpyFunc == nullptr)
        {
            return Msg();
        }
        handler.noRelpyFunc(msg);
    }
    return Msg();
}

bool MsgQ::checkMsgValid(const std::string& topic, const Msg& msg, std::unordered_map<std::string, std::string>& receipts)
{
    if(topic != msg.topic())
    {
        LOG_ERROR()  << __FUNCTION__ << " topic is not equal." << topic << "," << msg.topic();
        return false;
    }

    if(receipts.find(topic) != receipts.end())
    {
        if(receipts.at(topic) == msg.receipt())
        {
            LOG_ERROR()  << __FUNCTION__ << " This is duplicate msg." << receipts.at(topic) << "," << msg.receipt();
            return false;
        }
    }
    receipts[topic] = msg.receipt();
    return true;
}

}
