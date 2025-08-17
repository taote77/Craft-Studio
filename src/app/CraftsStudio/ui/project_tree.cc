#include "project_tree.h"
#include "engine/rs_scene_manager.h"
#include "model_manager.h"

#include <QAction>
#include <QChar>
#include <QDebug>
#include <QEvent>
#include <QInputDialog>
#include <QObject>
#include <QStandardItemModel>

ProjectTree::ProjectTree(QWidget* parent)
  : QTreeView(parent)
{
  // auto ff = new QStandardItemModel(this);

  ItemData root{ "root--", "模型" };

  const QStringList headers({ tr("项目") });

  _model = new TreeModel(this);

  // _model->setHorizontalHeaderLabels(QStringList() << tr("项目"));

  this->setModel(_model);

  // 初始化右键菜单
  _context_menu = new QMenu(this);
  QAction* renameAction = new QAction("重命名", this);
  connect(renameAction, &QAction::triggered, this, &ProjectTree::renameRootItem);
  _context_menu->addAction(renameAction);

  connect(this, &QTreeView::clicked, this, &ProjectTree::onItemActivated);
  connect(this, &QTreeView::activated, this, &ProjectTree::onItemSelected);

  // test datasets
  // _model->appendRow(ItemData("obj1", "模型1"));
  // _model->appendRow(ItemData("obj2", "模型2"));
  // auto root1 = _model->index(0, 0);
  //_model->appendRow(ItemData("obj1_1", "部件1-1"), root1);

  auto manager = SceneManager::getInstance();
  connect(manager, &SceneManager::rootObjectAdded, this, &ProjectTree::addRootNode);
  connect(manager, &SceneManager::objectRemoved, this, &ProjectTree::removeNode);
}

void ProjectTree::contextMenuEvent(QContextMenuEvent* event)
{
  if (this->currentIndex().isValid())
  {
    _context_menu->exec(event->globalPos());
  }
}

void ProjectTree::renameRootItem()
{
  QModelIndex currentIndex = this->currentIndex();
  if (!currentIndex.isValid())
  {
    return;
  }

  bool ok{ false };
  QString newName = QInputDialog::getText(dynamic_cast<QWidget*>(this), "重命名",
    "输入新名称:", QLineEdit::Normal, currentIndex.data().toString(), &ok);
  if (ok && !newName.isEmpty())
  {
    _model->setData(currentIndex, newName);
  }
}

void ProjectTree::addRootNode(SceneObject* obj)
{
  auto item_data = ItemData(obj->_id, obj->_name);
  _model->appendRow(item_data);

  // auto item = createNode(obj);
  // addTopLevelItem(item);
  // // 递归添加子节点
  // for (auto child : obj->children)
  // {
  //   addChildNode(item, child);
  // }
}

void ProjectTree::removeNode(SceneObject* obj)
{
  //
}

void ProjectTree::onItemActivated(const QModelIndex& index)
{
  //

  if (!index.isValid())
    return; // 检查索引是否有效

  // 获取选中项的数据
  QString itemText = index.data(Qt::DisplayRole).toString();
  qDebug() << "Clicked item:" << itemText;
}

void ProjectTree::onItemSelected(const QModelIndex& index)
{
  if (!index.isValid())
    return; // 检查索引是否有效

  // 获取选中项的数据
  QString itemText = index.data(Qt::DisplayRole).toString();
  qDebug() << "selected item:" << itemText;
}
