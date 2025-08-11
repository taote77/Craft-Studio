#include "project_tree.h"

ProjectTree::ProjectTree(QWidget* parent)
  : QDockWidget(parent)
{
  InitUi();
}

void ProjectTree::addWidget(QWidget* widget, int stretch, Qt::Alignment alignment)
{
  _vbox_layout->addWidget(widget, stretch, alignment);
}

void ProjectTree::InitUi()
{
  _container_widget = new QWidget(this);

  _vbox_layout = new QVBoxLayout(_container_widget);

  this->setAllowedAreas(Qt::LeftDockWidgetArea);
  this->setFeatures(QDockWidget::NoDockWidgetFeatures);

  _container_widget->setLayout(_vbox_layout);

  this->setWidget(_container_widget);
  this->setWindowTitle("Project Tree");
}