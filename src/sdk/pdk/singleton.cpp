#include "singleton.h"


namespace pdk
{

QString KeyHash( const QString& key, const QString& salt )
{
    QByteArray data;

    data.append( key.toUtf8() );
    data.append( salt.toUtf8() );
    data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();

    return data;
}


Singleton::Singleton( const QString& key )
    : key_( key )
    , lock_key_( KeyHash( key, "__lock_key__" ) )
    , mem_key_( KeyHash( key, "__mem_key__" ) )
    , mem_( mem_key_ )
    , lock_( lock_key_, 1 )
{
    lock_.acquire();
    {
        QSharedMemory fix( mem_key_ );
        fix.attach();
    }
    lock_.release();
}

Singleton::~Singleton()
{
    Release();
}

bool Singleton::IsLocked()
{
    if ( mem_.isAttached() )
    {
        return false;
    }

    lock_.acquire();

    const bool running = mem_.attach();

    if ( running )
    {
        mem_.detach();
    }

    lock_.release();

    return running;
}

bool Singleton::TryLock()
{
    if ( IsLocked() )
    {
        return false;
    }


    lock_.acquire();
    const bool result = mem_.create( sizeof( quint64 ) );
    lock_.release();

    if ( !result )
    {
        Release();
        return false;
    }

    return true;
}

void Singleton::Release()
{
    lock_.acquire();

    if ( mem_.isAttached() )
    {
        mem_.detach();
    }

    lock_.release();
}



}
