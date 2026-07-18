cmake_minimum_required(VERSION 3.20)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

enable_testing()

# One executable per test class (Qt Test standard pattern)
macro(add_slicingcore_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME}
        PRIVATE
            slicingcore
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Test
    )
    target_include_directories(${TEST_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

add_slicingcore_test(test_coord test_coord.cpp)
add_slicingcore_test(test_triangle_mesh test_triangle_mesh.cpp)
add_slicingcore_test(test_model_object test_model_object.cpp)
add_slicingcore_test(test_slice_pipeline test_slice_pipeline.cpp)
add_slicingcore_test(test_mesh_slicer test_mesh_slicer.cpp)
