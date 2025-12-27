project(q_clipper)

add_library(${PROJECT_NAME} MODULE)

set(SOURCES
    export.cc
    clipper_service.cc
    clipper_service.h
    clipper_widget.cc
    clipper_widget.h
    clipper_op_algorithm.h
    clipper_op_algorithm.cc
)


target_sources(${PROJECT_NAME} PRIVATE 
    ${SOURCES}
)

find_package(Clipper2 REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    qbus
    Clipper2::Clipper2
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS
    Concurrent Core REQUIRED
)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS 
    Concurrent Core REQUIRED
)

target_link_libraries( ${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/sdk/qbus/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX .mx
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUT_PATH}
)

# 添加编译选项来避免冲突
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-pedantic)
endif()

# -pedantic：启用严格的ISO标准警告
# -Wpedantic：等同于-pedantic（GCC的另一种写法）
# -Wno-pedantic：禁用-pedantic警告

