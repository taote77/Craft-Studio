#ifndef UZMQ_LOG_H
#define UZMQ_LOG_H

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <vector>

#define TARGET_LOG_TRACE(logger) BOOST_LOG_SEV(logger, boost::log::trivial::trace) <<BOOST_CURRENT_FUNCTION<<"|"
#define TARGET_LOG_DEBUG(logger) BOOST_LOG_SEV(logger, boost::log::trivial::debug) <<BOOST_CURRENT_FUNCTION <<"|"
#define TARGET_LOG_INFO(logger)  BOOST_LOG_SEV(logger, boost::log::trivial::info) <<BOOST_CURRENT_FUNCTION<<"|"
#define TARGET_LOG_WARN(logger)  BOOST_LOG_SEV(logger, boost::log::trivial::warning) <<BOOST_CURRENT_FUNCTION<<"|"
#define TARGET_LOG_ERROR(logger) BOOST_LOG_SEV(logger, boost::log::trivial::error) <<BOOST_CURRENT_FUNCTION<<"|"
#define TARGET_LOG_FATAL(logger) BOOST_LOG_SEV(logger, boost::log::trivial::fatal) <<BOOST_CURRENT_FUNCTION<<"|"

#define TARGET_LOG_TRACE_WITHOUT_FUNCTION(logger) BOOST_LOG_SEV(logger, boost::log::trivial::trace)
#define TARGET_LOG_DEBUG_WITHOUT_FUNCTION(logger) BOOST_LOG_SEV(logger, boost::log::trivial::debug)
#define TARGET_LOG_INFO_WITHOUT_FUNCTION(logger)  BOOST_LOG_SEV(logger, boost::log::trivial::info)
#define TARGET_LOG_WARN_WITHOUT_FUNCTION(logger)  BOOST_LOG_SEV(logger, boost::log::trivial::warning)
#define TARGET_LOG_ERROR_WITHOUT_FUNCTION(logger) BOOST_LOG_SEV(logger, boost::log::trivial::error)
#define TARGET_LOG_FATAL_WITHOUT_FUNCTION(logger) BOOST_LOG_SEV(logger, boost::log::trivial::fatal)


#define LOG_DEBUG()   TARGET_LOG_DEBUG(uzmq::defaultLogger())
#define LOG_INFO()    TARGET_LOG_INFO(uzmq::defaultLogger())
#define LOG_WARN()    TARGET_LOG_WARN(uzmq::defaultLogger())
#define LOG_ERROR()   TARGET_LOG_ERROR(uzmq::defaultLogger())

#define LOG_DEBUG_BRIEF TARGET_LOG_DEBUG_WITHOUT_FUNCTION(uzmq::defaultLogger())
#define LOG_INFO_BRIEF  TARGET_LOG_INFO_WITHOUT_FUNCTION(uzmq::defaultLogger())
#define LOG_WARN_BRIEF  TARGET_LOG_WARN_WITHOUT_FUNCTION(uzmq::defaultLogger())
#define LOG_ERROR_BRIEF TARGET_LOG_ERROR_WITHOUT_FUNCTION(uzmq::defaultLogger())


boost::log::record_ostream& operator<<(boost::log::record_ostream &os, std::vector<int> &value);
boost::log::record_ostream& operator<<(boost::log::record_ostream &os, std::vector<float> &value);
boost::log::record_ostream& operator<<(boost::log::record_ostream &os, std::vector<int64_t> &value);
boost::log::record_ostream& operator<<(boost::log::record_ostream &os, std::vector<std::string> &value);

namespace uzmq
{
    using Logger = boost::log::sources::severity_channel_logger<boost::log::trivial::severity_level, std::string>;

    Logger& defaultLogger();

    std::shared_ptr<Logger> creatLogger(const std::string& moduleName);
}



#endif // UZMQ_LOG_H
