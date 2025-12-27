cmake_minimum_required(VERSION 3.14)

project(CraftsStudio LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Gui Quick LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui Quick LinguistTools REQUIRED)

# set(TS_FILES cv-app_zh_CN.ts)

# === 2. 处理资源文件 (.qrc) ===
set(RESOURCES "")

if(${QT_VERSION_MAJOR} VERSION_EQUAL 6)
    qt6_add_resources(RESOURCES app.qrc)
elseif(${QT_VERSION_MAJOR} VERSION_EQUAL 5)
    qt5_add_resources(RESOURCES app.qrc)
endif()

add_executable(${PROJECT_NAME}
  main.cc
  engine/rs_interactor_trackball_camera.h
  engine/rs_interactor_trackball_camera.cc
  engine/rs_interactor_v2.h
  engine/rs_interactor_v2.cc
  engine/rs_scene_object.h
  engine/rs_scene_object.cc
  engine/rs_scene_object_v2.h
  engine/rs_scene_object_v2.cc
  engine/rs_scene_manager.h
  engine/rs_scene_manager.cc
  engine/rs_scene_manager_v2.h
  engine/rs_scene_manager_v2.cc
  engine/rs_transform_gizmo.h
  engine/rs_transform_gizmo.cc
  engine/build_platform.h
  engine/build_platform.cc
  engine/file_importer.h
  engine/file_importer.cc
  ui/mainwindow.h
  ui/mainwindow.cc
  ui/menu_manager.h
  ui/menu_manager.cc
  ui/project_tree.h
  ui/project_tree.cc
  ui/model_manager.h
  ui/model_manager.cc
  ui/tree_model.h
  ui/tree_model.cc
  ui/transform_panel.h
  ui/transform_panel.cc
  ui/plater_widget.h
  ui/plater_widget.cc
  action_factory.h
  action_factory.cc
  ${RESOURCES}
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

# if(NOT DEFINED VTK_QT_VERSION)
#   set(VTK_QT_VERSION 5)
# endif()


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
)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES}
)
