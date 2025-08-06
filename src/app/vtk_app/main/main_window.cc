#include "main_window.h"
#include "action_factory.h"
#include "action_group_manager.h"
#include "action_handler.h"
#include "documentview.h"
#include "widget_factory.h"

#include <QDebug>
#include <QEvent>
#include <QFileDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>

ApplicationWindow& ApplicationWindow::getAppWindow()
{
  static ApplicationWindow instance;
  return instance;
}

void ApplicationWindow::relayAction(QAction* q_action) {}

void ApplicationWindow::slotFileNewNew()
{
  auto doc_view = new GUI::DocumentView(QString("untitle"), _workspace);
  doc_view->setWindowTitle(QString("untitle_%1").arg(GUI::DocumentView::GetAssinIndex()));

  qDebug() << "new sub window title:" << doc_view->windowTitle();
  _workspace->addSubWindow(doc_view);
  // _workspace->setActiveSubWindow(doc_view);
}

void ApplicationWindow::slotWindowActivatedI(int index)
{
  if (index < 0 || index >= _workspace->subWindowList().size())
  {
    return;
  }
  slotWindowActivated(_workspace->subWindowList().at(index));
}

void ApplicationWindow::slotWindowActivated(QMdiSubWindow* sub_win)
{
  qDebug() << "activate windows";
}

void ApplicationWindow::slotFileNew()
{
  qDebug() << "new file";
  auto doc_view = new GUI::DocumentView(QString("untitle"), _workspace);
  doc_view->setWindowTitle(QString("untitle_%1").arg(GUI::DocumentView::GetAssinIndex()));

  qDebug() << "new sub window title:" << doc_view->windowTitle();
  _workspace->addSubWindow(doc_view);
}
void ApplicationWindow::slotFileOpen()
{
  qDebug() << "open file";
}
void ApplicationWindow::slotFileClose()
{
  qDebug() << "close file";
}
void ApplicationWindow::slotFileSave()
{
  //
  //
}
void ApplicationWindow::slotFileSaveAs()
{
  //
  //
}

void ApplicationWindow::slotImageProcess()
{
  // get image path
  auto img_path = QFileDialog::getOpenFileName(
    this, QString(""), QString("Image (*.png *.jpg *.bmp)"), QString(""));

  if (img_path.isEmpty())
  {
    return;
  }

  _action_handler->slotImageProcess(img_path);
  qDebug() << "open image:" << img_path;
}

void ApplicationWindow::slotSliceSTL()
{
  // get image path
  auto img_path = QFileDialog::getOpenFileName(
    this, QString(""), QString("Image (*.png *.jpg *.bmp)"), QString(""));

  if (img_path.isEmpty())
  {
    return;
  }

  _action_handler->slotSliceSTL(img_path);
  qDebug() << "open image:" << img_path;
}

void ApplicationWindow::InitUi()
{
  constexpr short width{ 1200 };
  constexpr short height{ 900 };
  this->resize(width, height);

  _workspace = new GUI::WorkSpace(this);
  this->setCentralWidget(_workspace);

  connect(_workspace, &QMdiArea::subWindowActivated, this, &ApplicationWindow::slotWindowActivated);

  setDockNestingEnabled(true);

  _doc_view = new GUI::DocumentView(
    QString("untitle_%1").arg(GUI::DocumentView::GetAssinIndex()), _workspace);

  _workspace->addSubWindow(_doc_view);

  _action_handler->setVtkRenderer(_doc_view->GetVtkRenderer(), _doc_view->GetVtkRenderWindow());

  _dock_dialog = new GUI::DockDialog(this);

  this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, _dock_dialog);

  _dock_dialog->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea);
  _dock_dialog->setFeatures(QDockWidget::NoDockWidgetFeatures);

  _console = new GUI::ConsoleWidget(this);
  this->addDockWidget(
    Qt::DockWidgetArea::BottomDockWidgetArea, _console, Qt::Orientation::Vertical);

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  GUI::ActionFactory action_factory(this, _action_handler);
  action_factory.fillActionContainer(_a_map, _ag_manager);

  GUI::WidgetFactory widget_factory(this, _a_map, _ag_manager);
  widget_factory.createSliceToolbars();

  widget_factory.createStandardToolbars(_action_handler);

  widget_factory.createMenus(menuBar());
}

ApplicationWindow::ApplicationWindow()
  : _ag_manager(new GUI::ActionGroupManager(this))
  , _action_handler(new GUI::ActionHandler(this))
{
  InitUi();
}
