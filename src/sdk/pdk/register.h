#ifndef ULTRABUS_REGISTER_H
#define ULTRABUS_REGISTER_H

#include <pdk/object.h>
#include <QMetaEnum>

namespace pdk {

bool PDK_API RegisterObject(Object* object,Object::ObjectFlag flag, void* handle);

bool PDK_API RegisterVersion(const QString& version,void* handle);

}

#define DECLARE_PLUGIN(ver) \
    extern "C" Q_DECL_EXPORT bool RegisterPlugin(void* __hdl__){\
    if(!pdk::RegisterVersion(QLatin1String(#ver),__hdl__)) return false;

///注册服务类型Object
#define REG_SERVICED_OBJECT(objtype) \
    if(!pdk::RegisterObject(new objtype,pdk::Object::ObjectFlag::Serviced,__hdl__)) return false;

///注册杂项Object
#define REG_MISC_OBJECT(objtype) \
    if(!pdk::RegisterObject(new objtype,pdk::Object::ObjectFlag::Misc,__hdl__)) return false;

///注册GUI Object
#define REG_GUI_OBJECT(objtype) \
    if(!pdk::RegisterObject(new objtype,pdk::Object::ObjectFlag::Gui,__hdl__)) return false;


#define DECLARE_PLUGIN_END()  return true;}


#endif // REGISTER_H
