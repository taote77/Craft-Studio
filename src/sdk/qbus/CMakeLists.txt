project(qbus)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED)


target_sources(${PROJECT_NAME}  PRIVATE 
    qbus_global.h
    service_export.h
    application.h
    application.cc
    micro_service.h
    micro_service.cc
    global_cxt.h
    global_cxt.cc
    service_manager.h
    service_manager.cc
    service_creator.h
    service_creator.cc
    service_bus.h
    service_bus.cc
    service_event.h
    dispatcher.h
    dispatcher.cc
)


find_package(QT NAMES Qt6 Qt5 COMPONENTS
    Concurrent
    DBus
    Gui
    Multimedia
    Network
    Qml
    Quick
    SerialPort
    Sql
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
    SerialPort
    Sql
    WebSockets
    Widgets
    Xml REQUIRED
)


target_link_libraries( ${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Xml
)


target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../
)



add_subdirectory(test)


# target_link_libraries(${PROJECT_NAME}  PUBLIC
#     protobuf
# )

