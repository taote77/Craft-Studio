cmake_minimum_required(VERSION 3.20)

# 查找所需的包
# find_package(Qt6 REQUIRED COMPONENTS Core Widgets)



find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui REQUIRED)


find_package(Clipper2 REQUIRED)

# 创建应用程序
add_executable(clipper_app
    main.cc
    mainwindow.cc
)

# 设置目标属性
target_compile_definitions(clipper_app PRIVATE QT_NO_KEYWORDS)

# 添加编译选项来避免冲突
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(clipper_app PRIVATE -Wno-pedantic)
endif()



# 链接库
target_link_libraries(clipper_app
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Clipper2::Clipper2
)

# 设置C++标准
target_compile_features(clipper_app PRIVATE cxx_std_17)

# 设置包含目录
target_include_directories(clipper_app 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)