#include "container.h"
#include <QDebug>
#include <QThreadPool>

namespace pdk {


Container::Container(const QString& path,QObject* parent)
    : QObject(parent)
    , root_(path)
{

}

bool Container::SetPath(const QString &filepath)
{
    QDir dir(filepath);

    if(!dir.exists())
    {
        return false;
    }

    root_ = dir;

    return true;
}

QString Container::Path() const
{
    return root_.path();
}

bool Container::Add(const QString &filename)
{
    QMutexLocker lock(&mutex_);

    if(!root_.exists(filename))
    {
        qWarning()<<filename<<"not exists!";
        return false;
    }

    if(blacklist_.contains(filename))
    {
        qWarning()<<"[blacklist] ignore"<<filename;
        return false;
    }

    if(plugins_.contains(filename))
    {
        qWarning()<<"plugin"<<filename<<"already register";
        return false;
    }

    auto plugin = new Plugin(root_.filePath(filename),this);

    if(false==plugin->Install())
    {
        qWarning()<<"plugin"<<filename<<"load failed";
        plugin->deleteLater();
        return false;
    }

    if(false==threads_.Assign(plugin))
    {
        qWarning()<<"plugin"<<filename<<"assign failed";
        threads_.Release(plugin);
        return false;
    }

    router_.Attacth(plugin);

    plugins_[plugin->FileInfo().fileName()] = plugin;
    return true;
}

bool Container::Remove(const QString &filename)
{
    QMutexLocker lock(&mutex_);

    auto plugin = plugins_.take(filename);
    if(!plugin)
    {
        return false;
    }

    router_.Detach(plugin);

    threads_.Release(plugin);

    return plugin->Uninstall();
}

void Container::AddAll()
{
    QStringList filters;

    filters<<"*.plugin";

    for(auto& filename: root_.entryList(filters))
    {
        Add(filename);
    }
}

void Container::RemoveAll()
{
    auto keys = plugins_.keys();
    for(auto& filename:keys)
    {
        Remove(filename);
    }
}

void Container::SetBlacklist(const QStringList &blacklist)
{
    blacklist_ = blacklist;
}


void Container::Cleanup()
{
    RemoveAll();

    threads_.Reset();

    qDebug()<<"plugins has been remove";
}

void Container::Dumps()
{
    qInfo()<<"************plugins tree************";
    for(auto&& plugin:plugins_)
    {
        qInfo()<<plugin->FileInfo().fileName()<<":"<<plugin->Version();
        for(auto&& obj:plugin->Objects())
        {
            qInfo()<<"    +--"<<obj->Path();
        }
    }
    qInfo()<<"*************************************";
}

void Container::Startup()
{
    AddAll();

    Dumps();

    auto threadpool =  QThreadPool::globalInstance();
    if(threadpool->maxThreadCount()<8)
    {
        qDebug()<<"default qtconcurrent thread count:"<<threadpool->maxThreadCount();
        threadpool->setMaxThreadCount(8);
        qDebug()<<"new qtconcurrent thread count:"<<threadpool->maxThreadCount();
    }

    threads_.Assign(&router_);

    threads_.Start();
}

QString Container::ReslovePath(const QString &filepath)
{
    if(QDir::isRelativePath(filepath))
    {
        return filepath;
    }

    return root_.filePath(filepath);
}



}

