#pragma once

#include "uzmq/object.h"
// #ifdef USE_UZMQ
// #else // USE_LIBPX
// #include "px/object.h"
// #endif

#if 1 // if include "proto_px.hpp" and include "ucore_define.h" then
#define PUBLISH_PX_CAST(topic,type,msg) publish(topic,uzmq::ZAny::valueFrom<type>(msg))
#define PUBLISH_WS_EVENT(msg) PUBLISH_PX_CAST(heygears::proto_px::GW_WS_EVENT, heygears::proto_uc::WebMessage, msg)
#define PUBLISH_WS_REPLY(msg) PUBLISH_PX_CAST(heygears::proto_px::GW_WS_REPLY, heygears::proto_uc::WebMessage, msg)
#endif