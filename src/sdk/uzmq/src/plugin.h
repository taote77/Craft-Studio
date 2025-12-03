#ifndef UZMQ_PLUGIN_H
#define UZMQ_PLUGIN_H

#include "uzmq/object.h"
#include "uzmq/log.h"
#include <boost/dll.hpp>
#include <boost/type_index.hpp>
#include <map>


namespace uzmq{


class Plugin
{
public:
    static Plugin& getInstance();

    bool install(const boost::filesystem::path& path);

    bool add(const std::shared_ptr<Object>& obj);

    int exec();

private:
    std::map<boost::filesystem::path, boost::dll::shared_library> mDlls;

    std::map<std::string,std::shared_ptr<Object>> mObjs;

private:
    Plugin(const Plugin &) = delete;
    Plugin &operator=(const Plugin &) = delete;
    Plugin();
    ~Plugin();
};

}
#endif // UZMQ_PLUGIN_H
