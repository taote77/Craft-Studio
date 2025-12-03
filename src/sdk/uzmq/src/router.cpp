#include "router.h"
#include "uzmq/log.h"

namespace uzmq
{

Router::Router(std::shared_ptr<zmq::context_t> ctx, const int& pubPort, const int& subPort, const std::string& comType)
    : mCtx(ctx)
    , mComType(comType)
{
    mPubAddr = mComType + std::to_string(pubPort);
    mSubAddr = mComType + std::to_string(subPort);
    mDireFuncs.clear();
}

Router::~Router()
{
    mXSub->close();
    mXPub->close();
    wait();
    mDireFuncs.clear();
    LOG_ERROR()  << "release proxy done";
}

bool Router::startUp()
{
    std::promise<bool> result;
    mProxyCtrl = std::async(std::launch::async, [&]()
    {
        try
        {
            LOG_INFO()  << "Starting proxy...";

            // Create mXSub XSUB socket
            mXSub = std::make_unique<zmq::socket_t>(*mCtx.lock(), zmq::socket_type::xsub);
            mXSub->bind(mSubAddr);

            // Create mXPub XPUB socket
            mXPub = std::make_unique<zmq::socket_t>(*mCtx.lock(), zmq::socket_type::xpub);
            mXPub->bind(mPubAddr);

            mXPub->set(zmq::sockopt::rcvhwm, mHightWaterLevel);
            mXPub->set(zmq::sockopt::sndhwm, mHightWaterLevel);
            mXSub->set(zmq::sockopt::rcvhwm, mHightWaterLevel);
            mXSub->set(zmq::sockopt::sndhwm, mHightWaterLevel);

            result.set_value(true);
            // Start the proxy
            zmq::proxy(*mXSub, *mXPub);
            // never reach here
        }
        catch(zmq::error_t& e)
        {
            result.set_value(false);
            LOG_ERROR()  << __FUNCTION__ << ":catch error:" << e.what();
        }
        return false;
    });
    return result.get_future().get();
}

bool Router::wait()
{
    if(false == mProxyCtrl.valid())
    {
        return -1;
    }

    return mProxyCtrl.get();
}

std::string Router::getPubAddr()
{
    return mPubAddr;
}

std::string Router::getSubAddr()
{
    return mSubAddr;
}

std::weak_ptr<zmq::context_t> Router::getZmqContext()
{
    return mCtx;
}

std::string Router::getComType()
{
    return mComType;
}

void Router::directlyAdd(const std::string &topic, Msg::ReplyFunc func)
{
    std::unique_lock<std::mutex> lock(mMutex);
    auto iter = mDireFuncs.find(topic);
    if(iter == mDireFuncs.end())
    {
        mDireFuncs.emplace(topic, func);
    }
}

Message::ReplyFunc Router::directlyFind(const std::string &topic)
{
    std::unique_lock<std::mutex> lock(mMutex);
    auto iter = mDireFuncs.find(topic);
    if(iter != mDireFuncs.end())
    {
        return iter->second;
    }
    else
    {
        return nullptr;
    }
}

void Router::directlyRemove(const std::string &topic)
{
    std::unique_lock<std::mutex> lock(mMutex);
    auto iter = mDireFuncs.find(topic);
    if(iter != mDireFuncs.end())
    {
        mDireFuncs.erase(iter);
    }
}
}    // namespace uzmq
