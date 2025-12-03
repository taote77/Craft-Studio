#ifndef UZMQ__APPINST_H
#define UZMQ__APPINST_H

#include "uzmq/log.h"
#include "router.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/process.hpp>
#include <regex>
#include <zmq.hpp>
#include <msgq.h>

namespace uzmq
{


class AppInst
{
public:
    static void setArgument(int argc, char* argv[]);

    static int exec();

    static  boost::property_tree::ptree properties();

    static void saveEnv(const boost::property_tree::ptree& env);

    static void initEnv(std::istringstream& stream);

    static const std::shared_ptr<Router>& getRouter();

    static std::shared_ptr<Logger>& getDefaultLogger();

    static boost::program_options::variable_value getOption(const std::string &key);

protected:
    AppInst();

    ~AppInst();


    void parseArgument(int argc, char* argv[]);

    void initEnviroment();

    void initEnviroment(std::istringstream& stream);

    void saveEnvToFile(const boost::property_tree::ptree &env);

    boost::property_tree::ptree getEnv();

    int run();

    void printEnv(const boost::property_tree::ptree &env);
private:
    static AppInst& entry();

    static std::string translate(const std::string& value);

private:
    bool scanAndLoad(const boost::filesystem::path &dir);

    bool load(const boost::filesystem::path &path);

private:
    std::shared_ptr<Logger> mDefaultLogger;

    std::shared_ptr<Router> mRouter =nullptr;

    std::shared_ptr<zmq::context_t> mZmpIo = nullptr;

    boost::property_tree::ptree mEnv;

    boost::program_options::variables_map mOptions;

    std::mutex mMutex;
};

}

#endif // UZMQ_APPINST_H
