#ifndef UZMQ_OBJECT_H
#define UZMQ_OBJECT_H

#include "log.h"
#include "message.h"
#include "platform.h"
#include <boost/format.hpp>

namespace uzmq
{
class  UZMQAPI Object
{
public:
    explicit Object();

    virtual ~Object();

    template<class T>
    static bool exportObj(std::string name)
    {
        return exportObj(std::make_shared<T>(), name);
    }
    static bool exportObj(const std::shared_ptr<Object>& object, std::string name);

protected:
    virtual void init();

    virtual void uninit();
public:
    template<class T>
    void connect(const int& topic,  void (T::*fn)(const Msg&), T* obj, bool block = false)
    {
        obj->bind(topic, std::bind(fn, obj, std::placeholders::_1), block);
    }

    template<class T>
    void connect(const std::string& topic,  void (T::*fn)(const Msg&), T* obj, bool block = false)
    {
        obj->bind(topic, std::bind(fn, obj, std::placeholders::_1), block);
    }

    template<class T>
    void connect(const int& topic,  ZAny (T::*fn)(const Msg&), T* obj, bool block = false)
    {
        obj->bind(topic, Msg::ReplyFunc(std::bind(fn, obj, std::placeholders::_1)), block);
    }

    template<class T>
    void connect(const std::string& topic,  ZAny (T::*fn)(const Msg&), T* obj, bool block = false)
    {
        obj->bind(topic, Msg::ReplyFunc(std::bind(fn, obj, std::placeholders::_1)), block);
    }

    template<class T>
    void connect(const int& topic, std::function<ZAny(const Msg&)> fn, T* obj, bool block = false) 
    {
        obj->bind(topic, Msg::ReplyFunc([fn](const Msg& msg) { return fn(msg); }), block);
    }

    template<class T>
    void connect(const std::string& topic, std::function<ZAny(const Msg&)> fn, T* obj, bool block = false) 
    {
        obj->bind(topic, Msg::ReplyFunc([fn](const Msg& msg) { return fn(msg); }), block);
    }

    void publish(const int& topic, const ZAny& data = {});
    void publish(const std::string& topic, const ZAny& data = {});

    Msg request(const int& topic, const ZAny& data = {}, const int& timeout = 5000);
    Msg request(const std::string& topic, const ZAny& data = {}, const int& timeout = 5000);

    Msg invoke(const int& topic, const ZAny& data = {});
    Msg invoke(const std::string& topic, const ZAny& data = {});

    void bind(const int& topic, Msg::NoReplyFunc func, bool block);
    void bind(const std::string& topic, Msg::NoReplyFunc func, bool block);

    void bind(const int& topic, Msg::ReplyFunc func, bool block);
    void bind(const std::string& topic, Msg::ReplyFunc func, bool block);

    void setObjName(std::string name);

    void startUp();

    void disconnect(const int& topic);

    void disconnect(const std::string& topic);

private:
    class Context;
    std::unique_ptr<Context> mContext;
};



#define DECLARE_PLUGIN() \
extern "C" UZMQAPI bool __main__(){
#define OBJECT_EXPORT(type) \
if(!uzmq::Object::exportObj<type>(#type)) return false;
#define DECLARE_PLUGIN_END()  return true;}

}
#endif // UZMQ_OBJECT_H
