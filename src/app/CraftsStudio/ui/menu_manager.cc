#include "menu_manager.h"
#include <QMenuBar>

MenuManager::MenuManager(QObject* parent)
  : QObject(parent)
{
}

void MenuManager::init(MainWindow& main_win, ActionFactory& action_fact)
{
  auto menu_bar = main_win.menuBar();

  // file menu
  auto menu_file = menu_bar->addMenu(tr("&File"));
  auto open_action = menu_file->addAction("&Open");

  auto open_stl_action = menu_file->addAction("Open STL File");
  connect(open_stl_action, &QAction::triggered, &action_fact, &ActionFactory::openSTLFile);

  auto open_obj_action = menu_file->addAction("Open OBJ File");
  connect(open_obj_action, &QAction::triggered, &action_fact, &ActionFactory::openOBJFile);

  menu_file->addSeparator();
  auto exit_action = menu_file->addAction("&Exit");
  connect(exit_action, &QAction::triggered, &action_fact, &ActionFactory::exitApp);

  // file menu end

  auto menu_edit = menu_bar->addMenu(tr("&Edit"));

  auto model_add = menu_edit->addMenu("Add Model");

  auto model_plane = model_add->addAction("Add Plane");
  connect(model_plane, &QAction::triggered, &action_fact, &ActionFactory::addPlane);

  auto model_sphere = model_add->addAction("Add Sphere");
  connect(model_sphere, &QAction::triggered, &action_fact, &ActionFactory::addSphere);

  auto model_cube = model_add->addAction("Add Cube");
  connect(model_cube, &QAction::triggered, &action_fact, &ActionFactory::addCube);

  // addCylinder
  auto model_cylinder = model_add->addAction("Add Cylinder");
  connect(model_cylinder, &QAction::triggered, &action_fact, &ActionFactory::addCylinder);

  auto model_cone = model_add->addAction("Add Cone");
  connect(model_cone, &QAction::triggered, &action_fact, &ActionFactory::addCone);

  auto model_clear = model_add->addAction("Clear Scene");
  connect(model_clear, &QAction::triggered, &action_fact, &ActionFactory::clearScene);

  auto menu_model = menu_bar->addMenu(tr("3D &Reconstruction"));
  auto merge_from_img_action = menu_model->addAction("Merge");
  connect(
    merge_from_img_action, &QAction::triggered, &action_fact, &ActionFactory::onConstructionFile);

  // auto open_obj_action = menu_model->addAction("Open OBJ File");
  // connect(open_obj_action, &QAction::triggered, &action_fact, &ActionFactory::openOBJFile);

  // add set add
}
