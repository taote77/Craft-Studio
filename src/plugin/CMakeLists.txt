cmake_minimum_required(VERSION 3.16)
project(plugins)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

include(GNUInstallDirs)



link_libraries(pdk)

link_directories(${CMAKE_SOURCE_DIR}/src/sdk)

message("dddddddddd" ${CMAKE_SOURCE_DIR}/src/sdk)

add_subdirectory(db)
add_subdirectory(sys)


# set_target_properties(
#   cs_db
#   cs_sys
#   PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIRECTORY})

# set_target_properties(
#   cs_db
#   cs_sys
#   PROPERTIES SUFFIX ".plugin")
