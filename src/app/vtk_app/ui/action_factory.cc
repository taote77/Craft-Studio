

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDebug>
// #include <qglobal.h>

#include "action_factory.h"
#include "action_group_manager.h"
#include "action_handler.h"
#include "main_window.h"

namespace GUI
{

ActionFactory::ActionFactory(ApplicationWindow* parent, ActionHandler* a_handler)
  : QObject(parent)
  , _main_window(parent)
  , _action_handler(a_handler)
{
}

void ActionFactory::fillActionContainer(
  QMap<QString, QAction*>& a_map, GUI::ActionGroupManager* agm)
{
  // <[~ File ~]>
  auto action = new QAction(ActionFactory::tr("&New"), agm->_file);
  action->setShortcut(QKeySequence::New);
  connect(action, &QAction::triggered, _main_window, &ApplicationWindow::slotFileNew);
  action->setObjectName("FileNew");
  a_map["FileNew"] = action;

  action = new QAction(ActionFactory::tr("&Open..."), agm->_file);
  action->setShortcut(QKeySequence::Open);
  connect(action, &QAction::triggered, _main_window, &ApplicationWindow::slotFileOpen);
  action->setObjectName("FileOpen");
  a_map["FileOpen"] = action;

  action = new QAction(ActionFactory::tr("&Close..."), agm->_file);
  action->setShortcut(QKeySequence::Open);
  connect(action, &QAction::triggered, _main_window, &ApplicationWindow::slotFileClose);
  action->setObjectName("FileClose");
  a_map["FileClose"] = action;

  action = new QAction(ActionFactory::tr("&Save"), agm->_file);
  action->setShortcut(QKeySequence::Save);
  connect(action, &QAction::triggered, _main_window, &ApplicationWindow::slotFileSave);
  action->setObjectName("FileSave");
  a_map["FileSave"] = action;

  action = new QAction(ActionFactory::tr("Save &as..."), agm->_file);
  action->setShortcut(QKeySequence::SaveAs);
  connect(action, &QAction::triggered, _main_window, &ApplicationWindow::slotFileSaveAs);
  action->setObjectName("FileSaveAs");
  a_map["FileSaveAs"] = action;

  action = new QAction(ActionFactory::tr("Exit"), agm->_file);
  connect(action, &QAction::triggered, this,
    []()
    {
      // save all files
      QApplication::quit();
    });

  action->setObjectName("Exit");
  a_map["Exit"] = action;

  // <[~ Modeling ~]>

  action = new QAction(ActionFactory::tr("Plane"), agm->_modeling);
  action->setObjectName("Plane");
  connect(action, &QAction::triggered, _action_handler, &ActionHandler::slotAddPlane);

  a_map["Plane"] = action;

  action = new QAction(ActionFactory::tr("Cone"), agm->_modeling);
  action->setObjectName("Cone");
  connect(action, &QAction::triggered, _action_handler, &ActionHandler::slotAddCone);

  a_map["Cone"] = action;

  action = new QAction(ActionFactory::tr("Cylinder"), agm->_modeling);
  action->setObjectName("Cylinder");
  connect(action, &QAction::triggered, _action_handler, &ActionHandler::slotAddCylinder);

  a_map["Cylinder"] = action;

  action = new QAction(ActionFactory::tr("ImgProcess"), agm->_modeling);
  action->setObjectName("ImgProcess");
  connect(action, &QAction::triggered, _main_window, &ApplicationWindow::slotImageProcess);

  a_map["ImgProcess"] = action;

  action = new QAction(ActionFactory::tr("SliceSTL"), agm->_modeling);
  action->setObjectName("SliceSTL");
  connect(action, &QAction::triggered, _main_window, &ApplicationWindow::slotSliceSTL);

  a_map["SliceSTL"] = action;
}

void ActionFactory::commonActions(QMap<QString, QAction*>& a_map, ActionGroupManager* agm) {}

} // namespace GUI