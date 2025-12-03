#ifndef UZMQ_LOGGER_H
#define UZMQ_LOGGER_H

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <string>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/filesystem.hpp>
#include <mutex>
#include <map>

namespace uzmq{

class LoggerInst
{
public:
    enum class LoggerType
    {
        ONLY_CONSOLE_LOG = 0,
        FILE_LOG,
     };

    using Logger = boost::log::sources::severity_channel_logger<boost::log::trivial::severity_level, std::string>;

public:
    static LoggerInst& getInstance();

    std::shared_ptr<Logger> createLogger(const std::string& moduleName, LoggerType type = LoggerType::FILE_LOG);

private:
    std::mutex mMutex;
    LoggerInst(const LoggerInst &) = delete;
    LoggerInst &operator=(const LoggerInst &) = delete;
    LoggerInst();
    ~LoggerInst();
};
}



#endif // UZMQLOGGER_H
