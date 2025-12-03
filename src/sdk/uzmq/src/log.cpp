#include "uzmq/log.h"
#include "appinst.h"
#include "loggerinst.h"

boost::log::record_ostream& operator<<(boost::log::record_ostream &os, std::vector<int> &value)
{
    for(auto item : value)
    {
        os << "," << item;
    }
    return os;
}

boost::log::record_ostream& operator<<(boost::log::record_ostream &os, std::vector<float> &value)
{
    for(auto item : value)
    {
        os << "," << item;
    }
    return os;
}

boost::log::record_ostream& operator<<(boost::log::record_ostream &os, std::vector<int64_t> &value)
{
    for(auto item : value)
    {
        os << "," << item;
    }
    return os;
}

boost::log::record_ostream& operator<<(boost::log::record_ostream &os, std::vector<std::string> &value)
{
    for(auto item : value)
    {
        os << "," << item;
    }
    return os;
}

namespace uzmq
{
auto console_logger  = LoggerInst::getInstance().createLogger("console", LoggerInst::LoggerType::ONLY_CONSOLE_LOG);

Logger &defaultLogger()
{
    std::weak_ptr<Logger> target = AppInst::getDefaultLogger();
    if(target.expired())
    {
        return *console_logger;
    }
    else
    {
        return *target.lock();
    }
}

std::shared_ptr<Logger> creatLogger(const std::string &moduleName)
{
    return LoggerInst::getInstance().createLogger(moduleName);
}

}
