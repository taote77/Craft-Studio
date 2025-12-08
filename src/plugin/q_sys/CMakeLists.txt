project(q_sys)

add_library(${PROJECT_NAME} MODULE)

set(SOURCES
    export.cc
    sys_service.cc
    sys_service.h)

target_sources(${PROJECT_NAME} PRIVATE 
    ${SOURCES}
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    qbus
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/sdk/qbus/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX .mx
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUT_PATH}
)

