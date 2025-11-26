#include "register.h"
#include "plugin.h"

namespace pdk {

bool PDK_API RegisterObject(Object *object,Object::ObjectFlag flag, void *handle)
{
    auto plugin = reinterpret_cast<Plugin*>(handle);

    return plugin->Alloc(object, flag);
}

bool PDK_API RegisterVersion(const QString &version, void *handle)
{
    auto plugin = reinterpret_cast<Plugin*>(handle);

    return plugin->SetVersion(version);
}

void RegisterTopics(const QStringList &topics, bool invokable, void *handle)
{
    auto plugin = reinterpret_cast<Plugin*>(handle);

    plugin->RegisterTopics(topics,invokable);
}



}

