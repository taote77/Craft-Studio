cmake_minimum_required(VERSION 3.20)

# ============================================================================
# libcsengine — Scene engine library with VTK integration
#
# Depends on: libslicingcore (data structures), VTK (rendering), Qt6::Core
# ============================================================================

find_package(VTK REQUIRED)

set(TARGET_NAME csengine)

set(SOURCES
    scene/scene_document.cpp
    scene/vtk_converter.cpp
    command/command.cpp
)

set(HEADERS
    csengine_global.h
    scene/scene_document.hpp
    command/command.hpp
)

add_library(${TARGET_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${TARGET_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..  # for #include <csengine/...>
        ${VTK_INCLUDE_DIRS}
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        slicingcore
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        ${VTK_LIBRARIES}
)

target_compile_definitions(${TARGET_NAME} PRIVATE CSENGINE_LIBRARY)

# Tests
add_subdirectory(test)
