#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>
#include <QLibrary>
#include <QSharedPointer>
#include <QMap>
#include <QFileInfo>

#include "object.h"


namespace pdk {

class Plugin: public QLibrary
{
    Q_OBJECT
public:
    Plugin(const QString& filepath,QObject* parent=nullptr);

    bool Install();

    bool Uninstall();

    bool Alloc(Object* object,Object::ObjectFlag flag);

    bool SetVersion(const QString& version);

    QString Version();

    void Clear();

    QFileInfo FileInfo() const;

    void RegisterTopics(const QStringList& topics,bool invokable);

    QList<QSharedPointer<Object>> Objects() const;

    void Ready();

signals:
    void SignalReady();

protected:
    using plugin_api_t = bool(*)(void*);

private:
    QMap<Address,QSharedPointer<Object>> objects_;

    QFileInfo fileinfo_;
};

}



#endif // PLUGIN_H
