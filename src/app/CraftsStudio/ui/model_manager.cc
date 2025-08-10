#include "model_manager.h"

ModelManager::ModelManager(QWidget* parent)
  : QTreeWidget(parent)
{
  // init model

  QStringList listLabels;
  listLabels << "模型管理";
  this->setHeaderLabels(listLabels);
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // 默认是适当显示滚动条，现状总是显示
  this->setColumnWidth(0, 150);
  this->setColumnWidth(1, 150);
  // add top tree item
  auto item = new QTreeWidgetItem(this);
  item->setText(0, "模型1");
  this->addTopLevelItem(item);

  item = new QTreeWidgetItem(this);
  item->setText(0, "模型2");
  this->addTopLevelItem(item);
}
