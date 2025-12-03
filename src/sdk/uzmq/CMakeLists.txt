cmake_minimum_required(VERSION 3.16)
project(uzmq LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB LIB_SRC
    ${PROJECT_SOURCE_DIR}/include/uzmq/*.h
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.cpp)

    
add_library(uzmq SHARED ${LIB_SRC})

if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64") #r818 交叉编译了暂时去除编译警告
target_compile_options(uzmq PRIVATE -fpermissive)
endif()


find_package(cppzmq REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem log_setup log json program_options serialization thread)
find_package(OpenCV REQUIRED)

target_link_libraries(uzmq ${Boost_LIBRARIES} ${OpenCV_LIBRARIES} cppzmq pthread dl rt)
set_target_properties(uzmq PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

target_compile_definitions(uzmq PRIVATE ZMQ_BUILD_DRAFT_API) #平台不一样可能会编译不过





