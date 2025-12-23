project(q_clipper)

add_library(${PROJECT_NAME} MODULE)

set(SOURCES
    export.cc
    clipper_service.cc
    clipper_service.h
    clipper_widget.cc
    clipper_widget.h
)


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

