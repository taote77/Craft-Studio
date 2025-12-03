#ifndef UZMQ_MSGQ_H
#define UZMQ_MSGQ_H

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <boost/asio.hpp>
#include "uzmq/message.h"
#include "router.h"

namespace uzmq
{
class MsgQ
{
public:
    struct HandleMsgType
    {
        std::shared_ptr<zmq::socket_t> socket;
        Msg::NoReplyFunc noRelpyFunc = nullptr;
        Msg::ReplyFunc RelpyFunc = nullptr;
        std::shared_ptr<boost::asio::thread_pool> thPool;
        bool block = false;
        bool replyFlag = false;
    };

    explicit MsgQ(std::shared_ptr<Router> router);

    virtual ~MsgQ();

    void setObjName(std::string name);

    int run(std::function<void(void)> init, std::function<void(void)> uninit);

    int wait();

    int close();

    void publish(const std::string &topic, const ZAny &data);

    Msg request(const std::string &topic, const ZAny &data, int timeout = 3000);

    Msg invoke(const std::string &topic, const ZAny &data);

    void add(const std::string& topic, Msg::NoReplyFunc func, bool block);

    void add(const std::string& topic, Msg::ReplyFunc func, bool block);

    void remove(const std::string& topic);

private:
    Msg::ErrorCode recvTimeout(const std::shared_ptr<zmq::socket_t>& socket, std::vector<zmq::message_t>& recv_msgs, int timeout);

    Msg sendWaitRet(const std::shared_ptr<zmq::socket_t> &socket, const Msg &msg, int timeout);

    bool sendReady(const std::shared_ptr<zmq::socket_t> &socket, int timeout = 1000);

    bool socketSend(const std::shared_ptr<zmq::socket_t>& socket, const Msg& msg);

    int msgLoop(std::function<void(void)> init, std::function<void(void)> uninit);

    void handleMsg(const Msg& msg);

    void runFuncInThread(Msg msg, HandleMsgType handler);

    Msg runFunc(const Msg& msg, HandleMsgType handler);

    bool checkMsgValid(const std::string& topic, const Msg& msg, std::unordered_map<std::string, std::string>& receipts);

private:
    std::map<std::string, HandleMsgType> mHandler;

    std::shared_ptr<zmq::socket_t> mPub;

    std::atomic_bool mStartFlag;

    std::future<int> mMsgLoopCtrl;

    zmq::poller_t<> mPoller;

    std::weak_ptr<Router> mRouter;

    std::weak_ptr<zmq::context_t> mCtx;

    std::mutex mHandlemutex;

    std::mutex mPubMutex;

    std::string mPubAddr;

    std::string mSubAddr;

    int mHightWaterLevel = 3000;

    std::string mComType = "inproc://";

    std::string mObjName;

    class ReplyMsgQueue
    {
    public:
        // 将套接字放入队列
        void push(const Msg& msg)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mReplyQ.push(msg);
        }

        bool pop(Msg& data)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            if(!mReplyQ.empty())
            {
                data = mReplyQ.front();
                mReplyQ.pop();
                return true;
            }
            return false;
        }

        void cleanup()
        {
            std::lock_guard<std::mutex> lock(mMutex);
            while (!mReplyQ.empty())
            {
                auto socket = mReplyQ.front();
                mReplyQ.pop();
            }
        }

    private:
        std::queue<Msg> mReplyQ;
        std::mutex mMutex;
    };

    ReplyMsgQueue mReplyMsgs;
};
}


#endif // UZMQ_MSGQ_H
