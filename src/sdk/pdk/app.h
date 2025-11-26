#ifndef PDK_APP_H
#define PDK_APP_H

#include <pdk/global.h>
#include <QObject>
#include <QDir>
#include <QApplication>

namespace pdk {


class PDK_API Application:public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

    virtual ~Application();

    void SetPluginDir(const QString& path);

    QDir PluginDir() const;

    QDir AppDir() const;

    void SetBlacklist(const QString& filepath);

signals:
    void Ready();

protected slots:
    bool TryLock() const;

    void Startup();

private:
    class Container* container_;
};

}



#endif // SERVER_H
