cmake_minimum_required(VERSION 3.20)

# ============================================================================
# libslicingcore — Core slicing engine library
#
# NO Qt::Widgets, NO VTK GUI, NO Qt3D dependencies.
# Only Qt::Core for QObject, signal/slot, threading primitives.
# Headless-capable, reusable by CLI, Backend, and tests.
# ============================================================================

set(TARGET_NAME slicingcore)

# --- Header-only geometry module ---
set(GEOMETRY_HEADERS
    geometry/coord.hpp
    geometry/ex_polygon.hpp
)

# --- Mesh module ---
set(MESH_SOURCES
    mesh/triangle_mesh.cpp
    mesh/model_object.cpp
)

set(MESH_HEADERS
    mesh/triangle_mesh.hpp
    mesh/model_object.hpp
)

# --- Slice module ---
set(SLICE_SOURCES
    slice/layer.cpp
    slice/mesh_slicer.cpp
)

set(SLICE_HEADERS
    slice/layer.hpp
    slice/mesh_slicer.hpp
)

# --- Pipeline module ---
set(PIPELINE_SOURCES
    pipeline/slice_pipeline.cpp
)

set(PIPELINE_HEADERS
    pipeline/slice_pipeline.hpp
)

# --- Library target ---
add_library(${TARGET_NAME} STATIC
    ${GEOMETRY_HEADERS}
    ${MESH_SOURCES}
    ${MESH_HEADERS}
    ${SLICE_SOURCES}
    ${SLICE_HEADERS}
    ${PIPELINE_SOURCES}
    ${PIPELINE_HEADERS}
    slicingcore_global.h
)

target_include_directories(${TARGET_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..  # for #include <slicingcore/...>
)

find_package(Clipper2 REQUIRED)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Clipper2::Clipper2
)

# Mark geometry as header-only for IDE integration
target_sources(${TARGET_NAME} PUBLIC FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/..
    FILES ${GEOMETRY_HEADERS} ${MESH_HEADERS} slicingcore_global.h
)

# --- Compile definitions ---
target_compile_definitions(${TARGET_NAME} PRIVATE SLICINGCORE_LIBRARY)

# --- Tests ---
add_subdirectory(test)
