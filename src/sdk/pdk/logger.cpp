#include "logger.h"

namespace pdk {

Logger::Logger(const char *category)
    :category_(category)
{

}

void Logger::SetEnabled(QtMsgType type, bool enabled)
{
    category_.setEnabled(type,enabled);
}

bool Logger::IsEnabled(QtMsgType type) const
{
    return category_.isEnabled(type);
}

QDebug Logger::Debug()
{
    return QMessageLogger().debug(category_);
}

QDebug Logger::Info()
{
    return QMessageLogger().debug(category_);
}

QDebug Logger::Warning()
{
    return QMessageLogger().warning(category_);
}

QDebug Logger::Critical()
{
    return QMessageLogger().critical(category_);
}


































}
