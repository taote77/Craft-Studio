set(SOURCES
    address.cpp
    container.cpp
    dispatcher.cpp
    logger.cpp
    message.cpp
    message_bus.cpp
    object.cpp
    plugin.cpp
    register.cpp
    router.cpp
    app.cpp
    singleton.cpp
    threads.cpp
    reply.cpp
    address.h
    container.h
    dispatcher.h
    global.h
    logger.h
    message.h
    message_bus.h
    object.h
    plugin.h
    register.h
    router.h
    app.h
    singleton.h
    threads.h
    reply.h)

add_library(pdk SHARED)
target_sources(pdk PRIVATE ${SOURCES})

target_include_directories(pdk PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../)

message(${CMAKE_CURRENT_LIST_DIR}  " dddddddddd")

target_link_libraries(pdk PUBLIC protobuf)
