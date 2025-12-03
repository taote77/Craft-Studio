#include "loggerinst.h"
#include "uzmq/env.h"
#include <boost/shared_ptr.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/dll.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
using namespace logging::trivial;


namespace uzmq
{

LoggerInst::LoggerInst()
{

}

LoggerInst::~LoggerInst()
{
    boost::shared_ptr<logging::core> core = logging::core::get();
    core->flush();
    core->remove_all_sinks();
}

LoggerInst &LoggerInst::getInstance()
{
    static LoggerInst instance;
    return instance;
}

std::shared_ptr<LoggerInst::Logger> LoggerInst::createLogger(const std::string& moduleName, LoggerType type)
{
    std::unique_lock<std::mutex> lock(mMutex);

    boost::shared_ptr<logging::core> core = logging::core::get();
    boost::log::formatter ftm = expr::stream
                                << "["
                                << expr::format_date_time<boost::posix_time::ptime>(
                                    "TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
                                << "] "
                                << "["
                                << logging::trivial::severity
                                << "] "
                                << "["
                                << expr::attr<std::string>("Channel")
                                << "] "
                                << "["
                                << expr::attr<attrs::current_thread_id::value_type>("ThreadID")
                                << "] "
                                << expr::smessage;

    if(type != LoggerType::ONLY_CONSOLE_LOG)
    {
        boost::filesystem::path defaultLogPath = ENV_VAR("log.dir", "/heygears/log/ucore");
        if (boost::filesystem::exists(defaultLogPath) == false)
        {
            defaultLogPath = boost::dll::program_location().parent_path() / "log";
        }
        auto folder = ENV_VAR("log.dir", defaultLogPath);
        auto rotationSize = ENV_VAR("log.rotation_size", 10 * 1024 * 1024);
        auto maxSize = ENV_VAR("log.max_size", 500 * 1024 * 1024);
        std::string str_level = ENV_VAR("log.level", "trace");
        logging::trivial::severity_level level = logging::trivial::trace;
        logging::trivial::from_string(str_level.c_str(), str_level.length(), level);

        boost::shared_ptr<sinks::text_file_backend> backend = boost::make_shared <sinks::text_file_backend >(
                    keywords::file_name = (folder / moduleName).string() + "_%3N_%Y%m%d_%H%M%S.log",
                    keywords::rotation_size = rotationSize,
                    keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0));
        backend->auto_flush(true);

        typedef sinks::synchronous_sink<sinks::text_file_backend> sink_t;
        boost::shared_ptr<sink_t> sink(new sink_t(backend));
        sink->locked_backend()->set_file_collector(sinks::file::make_collector(
                    keywords::target = folder.string(),
                    keywords::max_size = maxSize));
        sink->set_formatter(ftm);
        sink->set_filter(expr::attr<std::string>("Channel") == moduleName && logging::trivial::severity >= level);
        sink->locked_backend()->scan_for_files();
        core->add_sink(sink);
    }

    boost::shared_ptr< sinks::text_ostream_backend > c_backend = boost::make_shared< sinks::text_ostream_backend >();
    c_backend->add_stream(boost::shared_ptr< std::ostream >(&std::clog, boost::null_deleter()));
    c_backend->auto_flush(true);
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > c_sink_t;
    boost::shared_ptr< c_sink_t > c_sink(new c_sink_t(c_backend));
    c_sink->set_formatter(ftm);
    c_sink->set_filter(expr::attr<std::string>("Channel") == moduleName && logging::trivial::severity >= logging::trivial::trace);
    core->add_sink(c_sink);

    auto logger = std::make_shared<Logger>(keywords::channel = moduleName);

    logging::add_common_attributes();

    return logger;
}

}
