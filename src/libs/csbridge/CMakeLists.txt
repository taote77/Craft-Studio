cmake_minimum_required(VERSION 3.20)

# ============================================================================
# libcsbridge — Bridge layer between UI and slicing engine
#
# Provides SlicingController for running SlicePipeline on background thread.
# Depends on: libslicingcore, Qt6::Core
# ============================================================================

set(TARGET_NAME csbridge)

set(SOURCES
    slicing_controller.cpp
)

set(HEADERS
    csbridge_global.h
    slicing_controller.hpp
)

add_library(${TARGET_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${TARGET_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..  # for #include <csbridge/...>
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        slicingcore
        Qt${QT_VERSION_MAJOR}::Core
)

target_compile_definitions(${TARGET_NAME} PRIVATE CSBRIDGE_LIBRARY)
