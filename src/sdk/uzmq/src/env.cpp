#include "uzmq/env.h"
#include "appinst.h"

namespace uzmq{


void Environment::Preset(std::istringstream &stream)
{
    AppInst::initEnv(stream);
}

std::string Environment::Path()
{
    try
    {
        return AppInst::getOption("env").as<std::string>();
    }
    catch (const std::exception& e)
    {
        return std::string();
    }
}

boost::property_tree::ptree Environment::properties()
{
    return AppInst::properties();
}

void Environment::Save(const boost::property_tree::ptree& env)
{
    return AppInst::saveEnv(env);
}



};
