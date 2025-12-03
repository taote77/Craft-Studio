project(qdk)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(${PROJECT_NAME} SHARED)


target_sources(${PROJECT_NAME}  PRIVATE 
    global.h
    application.h
    application.cc
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
    ${CMAKE_CURRENT_LIST_DIR}/../
)

message(${CMAKE_CURRENT_LIST_DIR}  " CMAKE_CURRENT_LIST_DIR")

target_link_libraries(${PROJECT_NAME}  PUBLIC protobuf)
