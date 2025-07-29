cmake_minimum_required(VERSION 3.14)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake/global.cmake)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(MasterWay LANGUAGES C CXX)

add_subdirectory(src/app)

# set(VTK_DEBUG_MODULE_LOADER ON) # 跟踪模块加载顺序

add_subdirectory(src/3rd-party)

#project(lvglpp-editor LANGUAGES CXX)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

