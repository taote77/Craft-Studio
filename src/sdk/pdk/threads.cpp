#include "threads.h"
#include <QEventLoop>

namespace pdk {

Threads::Threads(int max_thread_count,QObject* parent):QObject(parent)
{
    for(int i=0;i<max_thread_count;i++)
    {
        idles_.push_back(new QThread(this));
    }

    misc_ = new QThread(this);
    misc_->setObjectName("misc");
}

Threads::~Threads()
{

}

bool Threads::Assign(Plugin *plugin)
{
    for(auto& obj: plugin->Objects())
    {
        if(!Assign(obj.data(),obj->Flag()))
        {
            return false;
        }

        QObject::connect(obj->thread(),&QThread::started,obj.data(),&Object::Run);
        QObject::connect(obj->thread(),&QThread::finished,obj.data(),&Object::Kill);
    }

    return true;
}

void Threads::Release(Plugin *plugin)
{
    for(auto& it: plugin->Objects())
    {
        auto obj = it.data();

        obj->AboutToKill();

        Release(obj);
    }
}


bool Threads::Assign(QObject *object, Object::ObjectFlag flag)
{
    QMutexLocker lock(&mutex_);

    if(flag==Object::ObjectFlag::Misc)
    {
        object->moveToThread(misc_);
        return true;
    }
    else if(flag==Object::ObjectFlag::Gui)
    {
        return true;
    }
    else if(idles_.empty())
    {
        return false;
    }

    auto t = idles_.takeFirst();

    object->moveToThread(t);
    t->setObjectName(object->metaObject()->className());

    serviced_.push_back(t);

    return true;
}

void Threads::Release(QObject *object)
{
    QMutexLocker lock(&mutex_);

    auto t = object->thread();

    if(serviced_.contains(t))
    {
        serviced_.removeAll(t);

        t->quit();

        t->wait();

        idles_.push_back(t);
    }
}

void Threads::Reset()
{
    QMutexLocker lock(&mutex_);

    for(auto& t:serviced_)
    {
        t->quit();

        t->wait();

        idles_.push_back(t);
    }

    misc_->quit();

    misc_->wait();
}

int Threads::Count(bool inused) const
{
    if(inused)
    {
        return serviced_.count() + 1;
    }

    return idles_.count();

}

void Threads::Start()
{
    QMutexLocker lock(&mutex_);

    misc_->start();

    for(auto& t:serviced_)
    {
        t->start();
    }
}

void Threads::Stop()
{
    QMutexLocker lock(&mutex_);

    misc_->quit();

    for(auto& t:serviced_)
    {
        t->quit();
    }
}

}


