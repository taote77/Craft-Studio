#ifndef THREADS_H
#define THREADS_H

#include <QThread>
#include <QMap>
#include <QMutexLocker>

#include "plugin.h"

namespace pdk {

class Threads:public QObject
{
    Q_OBJECT
public:
    Threads(int max_thread_count=32,QObject* parent=nullptr);

    ~Threads();

    bool Assign(Plugin* plugin);

    void Release(Plugin* plugin);

    void Start();

    void Stop();

    int Count(bool inused=true) const;

    bool Assign(QObject* object,Object::ObjectFlag flag = Object::ObjectFlag::Serviced);

    void Release(QObject* object);

    void Reset();

private:
    QList<QThread*> idles_;

    QList<QThread*> serviced_;

    QThread* misc_;

    QMutex mutex_;
};

}



#endif // THREADS_H
