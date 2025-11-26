#include "app.h"
#include "container.h"
#include "singleton.h"
#include <QDebug>
#include <QCoreApplication>
#include <QMetaType>
#include <QFile>
#include <QTimer>

namespace pdk {

Application::Application(int &argc, char **argv)
    :QApplication(argc,argv)
    ,container_(new Container(AppDir().filePath("plugin"),this))
{
    SetBlacklist(PluginDir().filePath(".blacklist"));

    QTimer::singleShot(1,this,&Application::Startup);
}

Application::~Application()
{

}

void Application::SetPluginDir(const QString &path)
{
    container_->SetPath(path);
}

QDir Application::PluginDir() const
{
    return container_->Path();
}

QDir Application::AppDir() const
{
    return applicationDirPath();
}

bool Application::TryLock() const
{
    static Singleton singleton(AppDir().filePath(".pdk"));
    return singleton.TryLock();
}

void Application::Startup()
{
    if(TryLock())
    {
        emit Ready();

        QObject::connect(qApp,&QCoreApplication::aboutToQuit,container_,&Container::Cleanup);
        QTimer::singleShot(1,container_,&Container::Startup);
    }
    else
    {
        qWarning("another instance is running...");
        QTimer::singleShot(1,this,&QCoreApplication::quit);
    }
}

void Application::SetBlacklist(const QString &filepath)
{
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text|QIODevice::ExistingOnly))
    {
        QStringList blacklist;
        while (!file.atEnd()) {
            auto name = file.readLine().trimmed();
            if(name.contains('#'))
            {
                name = name.left(name.indexOf('#')).trimmed();
            }

            if(name.isEmpty())
            {
                continue;
            }

            blacklist<<name.append(".plugin");
        }

        container_->SetBlacklist(blacklist);
    }
}


}

