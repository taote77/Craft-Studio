#ifndef ULTRABUS_CONTAINER_H
#define ULTRABUS_CONTAINER_H

#include <QObject>
#include <QDir>
#include <QMutex>
#include <QFileInfo>

#include "plugin.h"
#include "router.h"
#include "threads.h"

namespace pdk {


class Container : public QObject
{
    Q_OBJECT
public:
    explicit Container(const QString& path=QString(),QObject* parent=nullptr);

public slots:
    bool SetPath(const QString& filepath);

    QString Path() const;

    bool Add(const QString& filename);

    bool Remove(const QString& filename);

    void AddAll();

    void RemoveAll();

    void Dumps();

    void Startup();

    void Cleanup();

    void SetBlacklist(const QStringList& blacklist);


protected:
    QString ReslovePath(const QString& filepath);

signals:

private:
    Threads threads_;

    Router router_;

    QMap<QString,Plugin*> plugins_;

    QDir root_;

    QMutex mutex_;

    QStringList blacklist_;
};


}



#endif // ULTRABUS_CONTAINER_H
