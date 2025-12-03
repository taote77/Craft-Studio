project(uapp)

find_package(cppzmq REQUIRED)

add_executable(${PROJECT_NAME} main.cc)

target_include_directories(${PROJECT_NAME} PRIVATE
    ./common/
    
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    cppzmq
    uzmq    
    pthread
    dl
    rt
    ${Boost_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    BOOST_ALL_DYN_LINK
)