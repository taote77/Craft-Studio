project(qbus)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED)


set(QBUS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

target_sources(${PROJECT_NAME}  PRIVATE 
    ${QBUS_INCLUDE_DIR}/qbus/qbus_global.h
    ${QBUS_INCLUDE_DIR}/qbus/service_export.h
    ${QBUS_INCLUDE_DIR}/qbus/application.h
    ${QBUS_INCLUDE_DIR}/qbus/micro_service.h
    ${QBUS_INCLUDE_DIR}/qbus/global_cxt.h
    ${QBUS_INCLUDE_DIR}/qbus/service_manager.h
    application.cc
    micro_service.cc
    global_cxt.cc
    service_manager.cc
)


find_package(QT NAMES Qt6 Qt5 COMPONENTS
    Concurrent
    DBus
    Gui
    Multimedia
    Network
    Qml
    Quick
    SerialBus
    SerialPort
    Sql
    WebChannel
    WebSockets
    Widgets
    Xml REQUIRED
)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS 
    Concurrent
    DBus
    Gui
    Multimedia
    Network
    Qml
    Quick
    SerialBus
    SerialPort
    Sql
    WebChannel
    WebSockets
    Widgets
    Xml REQUIRED
)


target_link_libraries( ${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::SerialBus
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::WebChannel
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::WebSockets
)


target_include_directories(${PROJECT_NAME}  PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/include"
    ${CMAKE_CURRENT_LIST_DIR}
)

add_subdirectory(test)


# target_link_libraries(${PROJECT_NAME}  PUBLIC
#     protobuf
# )

