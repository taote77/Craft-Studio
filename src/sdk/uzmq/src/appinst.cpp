#include "appinst.h"

#include "loggerinst.h"
#include "plugin.h"
#include "uzmq/env.h"

#include <unistd.h>

namespace uzmq
{


void AppInst::setArgument(int argc, char* argv[])
{
    entry().parseArgument(argc, argv);
}

int AppInst::exec()
{
    return entry().run();
}

boost::property_tree::ptree AppInst::properties()
{
    return entry().getEnv();
}

void AppInst::saveEnv(const boost::property_tree::ptree& env)
{
    entry().saveEnvToFile(env);
}

void AppInst::initEnv(std::istringstream& stream)
{
    entry().initEnviroment(stream);
}

const std::shared_ptr<Router>& AppInst::getRouter()
{
    return entry().mRouter;
}

std::shared_ptr<Logger>& AppInst::getDefaultLogger()
{
    return entry().mDefaultLogger;
}

boost::program_options::variable_value AppInst::getOption(const std::string &key)
{
    return entry().mOptions["env"];
}

AppInst::AppInst()
{

}

AppInst::~AppInst()
{

}

void AppInst::parseArgument(int argc, char* argv[])
{
    try
    {
        boost::program_options::options_description desc("options");
        desc.add_options()("help,h", "help guide")("workspace,w", boost::program_options::value<std::string>(), "plugin dir")(
            "boost_log_setting,bls", boost::program_options::value<std::string>(), "path of boost log configure file")(
            "env,e", boost::program_options::value<std::string>(), "application global settings");

        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), mOptions);
        boost::program_options::notify(mOptions);

        for (auto& iter : mOptions)
        {
            LOG_INFO() << iter.first << "=" << iter.second.as<std::string>();
        }
    }
    catch (boost::program_options::error& e)
    {
        LOG_ERROR() << e.what();
    }
}

void AppInst::initEnviroment()
{
    boost::property_tree::ptree env;
    try
    {
        std::unique_lock<std::mutex> lock(mMutex);
        if (mOptions.count("env"))
        {
            auto ini = mOptions["env"].as<std::string>();
            boost::property_tree::ini_parser::read_ini(ini, env);

            for (auto& group : env)
            {
                for (auto& opt : group.second)
                {
                    auto key = group.first + "." + opt.first;
                    auto value = opt.second.data();
                    mEnv.put(key, value);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR() << e.what();
    }

    mDefaultLogger = LoggerInst::getInstance().createLogger("ucore");

    printEnv(env);
}

void AppInst::initEnviroment(std::istringstream& stream)
{
    std::unique_lock<std::mutex> lock(mMutex);
    try
    {
        boost::property_tree::ini_parser::read_ini(stream, mEnv);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR() << e.what();
    }
}

void AppInst::saveEnvToFile(const boost::property_tree::ptree& env)
{
    std::unique_lock<std::mutex> lock(mMutex);
    try
    {
        auto path = mOptions["env"].value();
        auto env_ini = boost::any_cast<std::string>(path);

        for (auto& group : env)
        {
            for (auto& opt : group.second)
            {
                auto key = group.first + "." + opt.first;
                auto value = opt.second.data();
                mEnv.put(key, value);
            }
        }

        boost::property_tree::write_ini(env_ini, mEnv);
        ::sync();    // 添加写入缓存同步,否则可能会出现配置文件数据丢失
    }
    catch (std::exception& e)
    {
        LOG_ERROR() << e.what();
    }
}

boost::property_tree::ptree AppInst::getEnv()
{
    std::unique_lock<std::mutex> lock(mMutex);
    return mEnv;
}

int AppInst::run()
{
    initEnviroment();

    mZmpIo = std::make_shared<zmq::context_t>(0);
    if (mZmpIo.get() == nullptr)
    {
        LOG_INFO() << "Failed to new ZmpIo";
        return -1;
    }

    int xSubPort = ENV_VAR("proxy.xsub_port", 65530);
    int xPubPort = ENV_VAR("proxy.xpub_port", 65531);
    std::string comType = ENV_VAR("proxy.com_type", "inproc://");
    mRouter = std::make_shared<Router>(mZmpIo, xSubPort, xPubPort, comType);
    if (mRouter.get() == nullptr)
    {
        LOG_INFO() << "Failed to new router";
        return -1;
    }
    if (!mRouter->startUp())
    {
        LOG_INFO() << "Failed to start up router";
        return -1;
    }

    std::string dir = (boost::dll::program_location().parent_path().parent_path() / "plugin/").string();
    if (mOptions.count("workspace"))
    {
        dir = mOptions["workspace"].as<std::string>();
        LOG_INFO() << "workspace is specified" << dir;
    }

    if (!scanAndLoad(dir))
    {
        return -1;
    }

    Plugin::getInstance().exec();

    return mRouter->wait();
}

bool AppInst::scanAndLoad(const boost::filesystem::path& dir)
{
    if (!boost::filesystem::exists(dir))
    {
        LOG_ERROR() << "workspace not exist:" << dir;
        return false;
    }

    if (!boost::filesystem::is_directory(dir))
    {
        LOG_ERROR() << "workspace is an invalid directoy:" << dir;
        return false;
    }

    for (const auto& iter : boost::filesystem::directory_iterator(dir))
    {
        if (!boost::filesystem::is_regular_file(iter.path()))
        {
            continue;
        }

        if (iter.path().extension() != ".uzmq")
        {
            continue;
        }
        load(iter.path());
    }

    return true;
}

bool AppInst::load(const boost::filesystem::path& path)
{
    if (!Plugin::getInstance().install(path))
    {
        return false;
    }
    else
    {
        LOG_INFO() << "success load plugin " << path;

        return true;
    }
}

void AppInst::printEnv(const boost::property_tree::ptree& env)
{
    LOG_INFO() << "-------------------------------------";
    for (auto& group : mEnv)
    {
        for (auto& opt : group.second)
        {
            auto key = group.first + "." + opt.first;
            if (env.get_child_optional(key) != boost::none)
            {
                LOG_INFO() << key << "=" << opt.second.get_value<std::string>() << "(ini)";
            }
            else
            {
                LOG_INFO() << key << "=" << opt.second.get_value<std::string>() << "(default)";
            }
        }
    }

    LOG_INFO() << "-------------------------------------";
}

AppInst& AppInst::entry()
{
    static AppInst entry;
    return entry;
}

std::string AppInst::translate(const std::string& input)
{
    static std::regex pattern(".*\\$\\{([A-Za-z0-9_]+)\\}.*");

    static std::smatch matches;
    if (std::regex_match(input, matches, pattern))
    {
        static const auto env = boost::this_process::environment();

        std::string correction(input);
        for (auto& var : matches)
        {
            if (env.count(var))
            {
                correction = boost::replace_first_copy(input, str(boost::format("${%s}") % var), env.at(var).to_string());
            }
        }

        return correction;
    }

    return input;
}

}    // namespace uzmq
