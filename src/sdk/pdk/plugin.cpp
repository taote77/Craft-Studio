#include "plugin.h"
#include <QMetaMethod>
#include <QThread>
#include <QDebug>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

template<class T>
static QString NameOf(const T& obj)
{
    QString classname = typeid (obj).name ();

#ifdef __GNUC__
    int status = 0 ;
    char* p = __cxxabiv1::__cxa_demangle( classname.toLatin1().data(), nullptr, nullptr, &status ) ;
    if( status == 0 )
        classname = p ;
    ::free(p) ;
#else //msvc
    classname.replace("class ","");
#endif
    return classname;
}

namespace pdk {


Plugin::Plugin(const QString& filepath,QObject* parent)
    :QLibrary(filepath,parent)
    ,fileinfo_(filepath)
{

}

bool Plugin::Install()
{
    if(false==load())
    {
        qWarning()<<errorString();
        return false;
    }

    auto register_plugin = reinterpret_cast<plugin_api_t>(resolve("RegisterPlugin"));

    if(nullptr==register_plugin)
    {
        qDebug()<<fileName()<<"is not a plugin";
        return false;
    }

    if(!register_plugin(this))
    {
        qDebug()<<"failed to install"<<fileName()<<":plugin register failed";
        return false;
    }

    return true;
}

bool Plugin::Uninstall()
{
    Clear();

    return unload();
}

bool Plugin::Alloc(Object *object,Object::ObjectFlag flag)
{
    auto address = QString("%1.%2").arg(fileinfo_.baseName(), NameOf(*object));

    if(objects_.contains(address))
    {
        return false;
    }

    object->Assign(address,flag);

    QObject::connect(this,&Plugin::SignalReady,object,&Object::Init,Qt::DirectConnection);

    if(flag==Object::ObjectFlag::Gui)
    {
        QObject::connect(this,&Plugin::SignalReady,object,&Object::Run,Qt::QueuedConnection);
    }


    objects_[address].reset(object,&QObject::deleteLater);

    return true;
}

bool Plugin::SetVersion(const QString &version)
{
    setProperty("version",version);
    return true;
}

QString Plugin::Version()
{
    return property("version").toString();
}

void Plugin::Clear()
{
    objects_.clear();
}

QFileInfo Plugin::FileInfo() const
{
    return fileinfo_;
}

void Plugin::RegisterTopics(const QStringList &topics, bool invokable)
{
    for(auto obj:objects_)
    {
        obj->Preset(topics,invokable);
    }
}


QList<QSharedPointer<Object>> Plugin::Objects() const
{
    return objects_.values();
}

void Plugin::Ready()
{
    emit SignalReady();
}

}




