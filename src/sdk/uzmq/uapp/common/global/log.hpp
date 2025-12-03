#pragma once


#include <string>
#include <cstdio>

#if defined(UCORE) && defined(UZMQ_LOG_OBJECT)
#include "uzmq/log.h"
#define LogDebug LOG_DEBUG()
#define LogInfo  LOG_INFO()
#define LogWarn  LOG_WARN() 
#define LogError LOG_ERROR()

#define MOD_LOG_DEBUG() LOG_DEBUG()
#define MOD_LOG_INFO() LOG_INFO()
#define MOD_LOG_WARN() LOG_WARN() 
#define MOD_LOG_ERROR() LOG_ERROR()

#else

#include <boost/log/trivial.hpp>

#define LogTrace BOOST_LOG_TRIVIAL(trace)<<BOOST_CURRENT_FUNCTION<<"|"<< __LINE__<<": "
#define LogDebug BOOST_LOG_TRIVIAL(debug)<<BOOST_CURRENT_FUNCTION<<"|"<< __LINE__<<": "
#define LogInfo  BOOST_LOG_TRIVIAL(info)<<BOOST_CURRENT_FUNCTION<<"|"<< __LINE__<<": "
#define LogWarn  BOOST_LOG_TRIVIAL(warning)<<BOOST_CURRENT_FUNCTION<<"|"<< __LINE__<<": "
#define LogError BOOST_LOG_TRIVIAL(error)<<BOOST_CURRENT_FUNCTION<<"|"<< __LINE__<<": "
#define LogFatal BOOST_LOG_TRIVIAL(fatal)<<BOOST_CURRENT_FUNCTION<<"|"<< __LINE__<<": "

#define MOD_LOG_DEBUG() BOOST_LOG_TRIVIAL(debug) << BOOST_CURRENT_FUNCTION << "|"
#define MOD_LOG_INFO() BOOST_LOG_TRIVIAL(info) << BOOST_CURRENT_FUNCTION << "|"
#define MOD_LOG_WARN() BOOST_LOG_TRIVIAL(warning) << BOOST_CURRENT_FUNCTION << "|"
#define MOD_LOG_ERROR() BOOST_LOG_TRIVIAL(error) << BOOST_CURRENT_FUNCTION << "|"

#define LOG_TRACE_BRIEF BOOST_LOG_TRIVIAL(trace)<<"|"
#define LOG_DEBUG_BRIEF BOOST_LOG_TRIVIAL(debug)<<"|"
#define LOG_INFO_BRIEF  BOOST_LOG_TRIVIAL(info)<<"|"
#define LOG_WARN_BRIEF  BOOST_LOG_TRIVIAL(warning)<<"|"
#define LOG_ERROR_BRIEF BOOST_LOG_TRIVIAL(error)<<"|"
#define LOG_FATAL_BRIEF BOOST_LOG_TRIVIAL(fatal)<<"|"

#endif

#define LOG_BT LOG_TRACE_BRIEF
#define LOG_BD LOG_DEBUG_BRIEF
#define LOG_BI LOG_INFO_BRIEF
#define LOG_BW LOG_WARN_BRIEF
#define LOG_BE LOG_ERROR_BRIEF
#define LOG_BF LOG_FATAL_BRIEF

#define LOG_BTF LOG_TRACE_BRIEF << __FUNCTION__ <<" "
#define LOG_BDF LOG_DEBUG_BRIEF << __FUNCTION__ <<" "
#define LOG_BIF LOG_INFO_BRIEF << __FUNCTION__<<" "
#define LOG_BWF LOG_WARN_BRIEF << __FUNCTION__<<" "
#define LOG_BEF LOG_ERROR_BRIEF << __FUNCTION__<<" "
#define LOG_BFF LOG_FATAL_BRIEF << __FUNCTION__<<" "

#include <string>
#include <sstream>
#include <mutex>

namespace log_utils {

template <typename T>
std::string combination(const std::string key, const T value)
{
    std::stringstream os;
    os << "#" << key << ":" << value << "#";
    return os.str();
}


template <typename T>
void join(std::stringstream &ss, const T item)
{
    ss << item << "#";
}

template<typename T, typename... Args>
void join(std::stringstream &ss, T item, Args... args)
{
    ss << item << ",";
    join(ss,args...);
}

template<typename... Args>
std::string customJoin(Args... args)
{
    std::stringstream ss;
    ss << "#";
    join(ss, args...);

    std::string temp(ss.str());
    bool key_colon = true;
    for(auto& item : temp)
    {
        if (item == ',')
        {
            if (key_colon)
            {
                item = ':';
            }
            key_colon = !key_colon;
        }
    }
    return temp;
}

template<typename... Args>
void printLog(Args... args)
{
    LOG_BI << customJoin(args...);
}

} // namespace log_utils