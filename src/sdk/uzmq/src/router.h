#ifndef UZMQ_ROUTER_H
#define UZMQ_ROUTER_H

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <future>
#include "uzmq/message.h"

namespace uzmq
{
class Router
{
public:
    explicit Router(std::shared_ptr<zmq::context_t> ctx, const int& pubPort, const int& subPort, const std::string& comType = "inproc://");

    virtual ~Router();

    bool startUp();

    bool wait();

    std::string getPubAddr();

    std::string getSubAddr();

    std::weak_ptr<zmq::context_t> getZmqContext();

    std::string getComType();

    void directlyAdd(const std::string &topic, Msg::ReplyFunc func);

    Msg::ReplyFunc directlyFind(const std::string &topic);

    void directlyRemove(const std::string &topic);
private:

    std::weak_ptr<zmq::context_t> mCtx;

    std::string mComType = "inproc://";

    std::string mPubAddr;

    std::string mSubAddr;

    std::unique_ptr<zmq::socket_t> mXSub;

    std::unique_ptr<zmq::socket_t> mXPub;

    std::future<bool> mProxyCtrl;

    int mHightWaterLevel = 5000;

    std::map<std::string, Msg::ReplyFunc> mDireFuncs;

    std::mutex mMutex;
};
}    // namespace uzmq

#endif // ROUTER_H
