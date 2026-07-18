cmake_minimum_required(VERSION 3.20)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(TEST_TARGET csengine_test)

add_executable(${TEST_TARGET}
    main.cpp
    # test source files are included by main.cpp
)

target_link_libraries(${TEST_TARGET}
    PRIVATE
        csengine
        slicingcore
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
        ${VTK_LIBRARIES}
)

target_include_directories(${TEST_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

enable_testing()
add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
