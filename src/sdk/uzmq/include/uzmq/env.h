#ifndef UZMQ_ENV_H
#define UZMQ_ENV_H


#include "uzmq/platform.h"
#include <boost/property_tree/ptree.hpp>

namespace uzmq{

class UZMQAPI Environment
{
public:
    template<class T>
    static inline T Variable(const std::string& name, const T& default_value=T())
    {
        return properties().get<T>(name,default_value);
    }

    static inline std::string Variable(const std::string& name, const std::string& default_value=std::string())
    {
        return properties().get<std::string>(name,default_value);
    }

    template<class T>
    static inline void Update(const std::string& name, const T& value,bool saved=true)
    {
        boost::property_tree::ptree env;
        env.put<T>(name,value);
        if(saved)
        {
           Save(env);
        }
    }

    static void Preset(std::istringstream& stream);

    static std::string Path();
    
private:
    static boost::property_tree::ptree properties();

    static void Save(const boost::property_tree::ptree& env);
};


}

#define ENV_VAR  uzmq::Environment::Variable

#define SET_ENV_VAR uzmq::Environment::Update

#define PRESET_ENV_VAR uzmq::Environment::Preset

#define ENV_PATH uzmq::Environment::Path

#endif // UZMQ_ENV_H
