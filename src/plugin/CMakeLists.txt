cmake_minimum_required(VERSION 3.16)
project(plugins)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

include(GNUInstallDirs)


link_libraries(pdk)

link_directories(${CMAKE_SOURCE_DIR}/src/sdk)

add_subdirectory(db)
add_subdirectory(sys)

add_subdirectory(q_sys)
add_subdirectory(q_cloud)
add_subdirectory(q_clipper)

