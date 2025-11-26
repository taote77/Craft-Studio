project(cs_db)

add_library(${PROJECT_NAME} MODULE)

set(SOURCES
    export.cc
    db.cc
    db.h)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# target_include_directories(${PROJECT_NAME} PRIVATE ./)

# target_link_libraries(hg_cloud PRIVATE PRIVATE global_env ultracore-proto)


set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX .plugin
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUT_PATH}
)
