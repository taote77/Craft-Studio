#ifndef ULTRABUS_LOGGER_H
#define ULTRABUS_LOGGER_H

#include <QMessageLogger>
#include <QLoggingCategory>
#include <QDebug>

namespace pdk {

class Logger
{
public:
    Logger(const char* category);

    void SetEnabled(QtMsgType type,bool enabled);

    bool IsEnabled(QtMsgType type) const;

    QDebug Debug();

    QDebug Info();

    QDebug Warning();

    QDebug Critical();

    template<typename ..._Ty>
    void Fatal(_Ty ...args)
    {
        QMessageLogger().fatal(args ...);
    }

private:
    QLoggingCategory category_;
};


}

#endif // LOGGING_H
