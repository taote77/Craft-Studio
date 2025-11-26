cmake_minimum_required(VERSION 3.14)

project(CraftsStudioBackend LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Gui Quick LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui Quick LinguistTools REQUIRED)

# set(TS_FILES cv-app_zh_CN.ts)

add_executable(${PROJECT_NAME}
  main.cc
)

# target_compile_definitions(${PROJECT_NAME}  PRIVATE 
#   $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
# )

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Quick
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Gui
)

# qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

find_package(VTK REQUIRED)

if(NOT VTK_FOUND)
  message(FATAL_ERROR "MinimalQtVTKApp: Unable to find the VTK build folder.")
endif()

if(NOT(TARGET VTK::GUISupportQt))
  message(FATAL_ERROR "MinimalQtVTKApp: VTK not built with Qt support.")
endif()

if(NOT DEFINED VTK_QT_VERSION)
  set(VTK_QT_VERSION 5)
endif()


find_package(TBB REQUIRED)

if(NOT TBB_FOUND)
    message(FATAL_ERROR "Unable to find the TBB")
else()
    message("Found TBB ")
    target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)
endif()



if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
  cmake_policy(SET CMP0071 NEW)
endif()

message(STATUS "VTK_VERSION: ${VTK_VERSION}")

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${VTK_LIBRARIES}  
  pdk
)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES}
)



target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/src/sdk
)