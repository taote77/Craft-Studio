#ifndef ULTRABUS_SINGLETON_H
#define ULTRABUS_SINGLETON_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QCryptographicHash>

namespace pdk {


class Singleton
{

public:
    Singleton( const QString& key);

    ~Singleton();

    bool IsLocked();

    bool TryLock();

    void Release();

private:
    const QString key_;

    const QString lock_key_;

    const QString mem_key_;

    QSharedMemory mem_;

    QSystemSemaphore lock_;

    Q_DISABLE_COPY( Singleton )
};

}


#endif // SINGLETON_H
