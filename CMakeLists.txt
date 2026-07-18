cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(MasterWay LANGUAGES C CXX)


include(cmake/build.cmake)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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


link_libraries(
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


add_subdirectory(src/app)

add_subdirectory(src/plugin)

add_subdirectory(src/sdk)

add_subdirectory(src/libs)

# set(VTK_DEBUG_MODULE_LOADER ON) # 跟踪模块加载顺序

# add_subdirectory(src/3rd-party)
