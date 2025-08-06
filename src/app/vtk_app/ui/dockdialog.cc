#include "dockdialog.h"
#include <qboxlayout.h>
#include <qtabwidget.h>

namespace GUI
{

DockDialog::DockDialog(QWidget* parent)
  : QDockWidget(parent)
{
  InitUI();
}

void DockDialog::InitUI()
{
  this->setWindowTitle("Solution");
  this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  _table_widget = new QTabWidget(this);

  QWidget* tab1 = new QWidget;
  _table_widget->addTab(tab1, "tab1");

  this->setWidget(_table_widget);

  auto features = this->features();
  features = features & ~QDockWidget::DockWidgetClosable;
  this->setFeatures(features);
}

} // namespace GUI
