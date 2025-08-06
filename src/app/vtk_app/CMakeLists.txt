cmake_minimum_required(VERSION 3.21)

project(CraftsStudio LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Concurrent  LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Concurrent LinguistTools REQUIRED)

# find_package(VTK COMPONENTS
#   CommonCore
#   GUISupportQt
#   InfovisCore
#   RenderingFreeType
#   RenderingCore
#   RenderingOpenGL2 
#   ViewsQt)


find_package(VTK REQUIRED)


if(NOT VTK_FOUND)
  message("Skipping example: ${VTK_NOT_FOUND_MESSAGE}")
  return()
endif()

message(STATUS "QT_VERSION_MAJOR: ${QT_VERSION}")

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
  main
  ui
  action
  engine
)

qt5_add_resources(qrc_FILES app.qrc)

#qt_add_resources(QT_RCC_F  app.qrc)


target_sources(${PROJECT_NAME} PRIVATE
  main/main.cc
  main/application.cc
  main/application.h
  main/main_window.h
  main/main_window.cc
  ui/action_factory.h
  ui/action_factory.cc
  ui/action_handler.h
  ui/action_handler.cc
  ui/action_group_manager.h
  ui/action_group_manager.cc
  ui/widget_factory.h
  ui/widget_factory.cc
  ui/workspace.cc
  ui/workspace.h
  ui/dockdialog.cc
  ui/dockdialog.h
  ui/documentview.cc
  ui/documentview.h
  ui/consolewidget.cc
  ui/consolewidget.h
  action/action_interface.h
  action/action_interface.cc
  action/base_type.h
  action/base_type.cc
  engine/rs_document.h
  engine/rs_document.cc
  engine/rs_interactor_style.h
  engine/rs_interactor_style.cc
  engine/rs_interactorstyle_trackcamera.h
  engine/rs_interactorstyle_trackcamera.cc
  ${qrc_FILES}
)


target_link_libraries(${PROJECT_NAME} PRIVATE 
  ${VTK_LIBRARIES}
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Concurrent
)

