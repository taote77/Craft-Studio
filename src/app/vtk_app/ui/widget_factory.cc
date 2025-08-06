
#include <QActionGroup>
#include <QDesktopServices>
#include <QFile>
#include <QMenu>
#include <QMenuBar>
#include <QObject>
#include <QStatusBar>
#include <QToolBar>

#include "action_factory.h"
#include "action_group_manager.h"
#include "action_handler.h"
#include "main_window.h"
#include "widget_factory.h"

namespace GUI
{

WidgetFactory::WidgetFactory(
  ApplicationWindow* main_win, QMap<QString, QAction*>& action_map, ActionGroupManager* agm)
  : QObject(nullptr)
  , _main_window(main_win)
  , a_map(action_map)
  , _ag_manager(agm)

{
  _file_actions << a_map["FileNew"] << a_map["FileOpen"] << a_map["FileClose"] << a_map["FileSave"]
                << a_map["FileSaveAs"] << a_map["Exit"];

  _model_actions << a_map["Plane"] << a_map["Cone"] << a_map["Cylinder"];

  // _help_actions
  //     << a_map["Help"]
  //     << a_map["About"]
  //     << a_map["Version"]
}

void WidgetFactory::createStandardToolbars(ActionHandler* action_handler)
{
  // QSizePolicy toolBarPolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // auto file_toolbar = new QToolBar(ApplicationWindow::tr("File"), _main_window);
  // file_toolbar->setSizePolicy(toolBarPolicy);
  // file_toolbar->setObjectName("file_toolbar");
  // file_toolbar->addActions(_file_actions);
}

void WidgetFactory::createSliceToolbars()
{
  // TODO
}

void WidgetFactory::createMenus(QMenuBar* menu_bar)
{
  // <[~ File ~]>
  auto file_menu = new QMenu(ApplicationWindow::tr("&File"), menu_bar);
  file_menu->setObjectName("File");
  // file_menu->setTearOffEnabled(true);
  file_menu->addAction(a_map["FileNew"]);
  file_menu->addAction(a_map["FileOpen"]);
  file_menu->addSeparator();

  file_menu->addAction(a_map["FileClose"]);
  file_menu->addSeparator();

  file_menu->addAction(a_map["FileSave"]);
  file_menu->addAction(a_map["FileSaveAs"]);
  file_menu->addSeparator();
  file_menu->addAction(a_map["Exit"]);

  menu_bar->addMenu(file_menu);

  // <[~ Model ~]>
  auto menu = new QMenu(ApplicationWindow::tr("&Model"), menu_bar);
  menu->addAction(a_map["Plane"]);
  menu->addAction(a_map["Cone"]);
  menu->addAction(a_map["Cylinder"]);

  menu_bar->addMenu(menu);

  menu = new QMenu(ApplicationWindow::tr("PostProcess"), menu_bar);
  menu->addAction(a_map["ImgProcess"]);
  menu->addAction(a_map["SliceSTL"]);

  menu_bar->addMenu(menu);

  menu = new QMenu(ApplicationWindow::tr("&Help"), menu_bar);
  menu_bar->addMenu(menu);
}

void WidgetFactory::createStatusBar(QStatusBar* menu_bar)
{
  menu_bar->showMessage(ApplicationWindow::tr("Ready"));
}

} // namespace GUI