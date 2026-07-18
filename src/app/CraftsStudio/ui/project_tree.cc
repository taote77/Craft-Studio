#include "project_tree.h"

#include <csengine/scene/scene_document.hpp>
#include <slicingcore/mesh/model_object.hpp>

#include <QAction>
#include <QChar>
#include <QDebug>
#include <QEvent>
#include <QInputDialog>
#include <QObject>

ProjectTree::ProjectTree(QWidget* parent)
  : QTreeView(parent)
{
  const QStringList headers({ tr("项目") });
  _model = new TreeModel(this);
  this->setModel(_model);

  // 初始化右键菜单
  _context_menu = new QMenu(this);
  QAction* renameAction = new QAction("重命名", this);
  connect(renameAction, &QAction::triggered, this, &ProjectTree::renameRootItem);
  _context_menu->addAction(renameAction);

  connect(this, &QTreeView::clicked, this, &ProjectTree::onItemActivated);
  connect(this, &QTreeView::activated, this, &ProjectTree::onItemSelected);
}

void ProjectTree::setSceneDocument(csengine::SceneDocument* doc)
{
  _sceneDocument = doc;
  if (_sceneDocument)
  {
    connect(_sceneDocument, &csengine::SceneDocument::objectsAdded,
            this, [this](const QList<slicing::ModelObject*>& objects) {
              for (auto* obj : objects)
                addRootNode(obj);
            });
    connect(_sceneDocument, &csengine::SceneDocument::objectsRemoved,
            this, &ProjectTree::removeModelNode);
  }
}

void ProjectTree::contextMenuEvent(QContextMenuEvent* event)
{
  if (this->currentIndex().isValid())
    _context_menu->exec(event->globalPos());
}

void ProjectTree::renameRootItem()
{
  QModelIndex currentIndex = this->currentIndex();
  if (!currentIndex.isValid()) return;

  bool ok{ false };
  QString newName = QInputDialog::getText(dynamic_cast<QWidget*>(this), "重命名",
    "输入新名称:", QLineEdit::Normal, currentIndex.data().toString(), &ok);
  if (ok && !newName.isEmpty())
    _model->setData(currentIndex, newName);
}

void ProjectTree::addRootNode(slicing::ModelObject* obj)
{
  ItemData itemData(QString::number(obj->id()), QString::fromStdString(obj->name()));
  _model->appendRow(itemData);
}

void ProjectTree::removeModelNode()
{
  // Rebuild the tree from current state
  if (!_sceneDocument) return;
  _model = new TreeModel(this);
  this->setModel(_model);
  for (auto* obj : _sceneDocument->objects())
    addRootNode(obj);
}

void ProjectTree::onItemActivated(const QModelIndex& index)
{
  if (!index.isValid()) return;
  QString itemText = index.data(Qt::DisplayRole).toString();
  qDebug() << "Clicked item:" << itemText;
}

void ProjectTree::onItemSelected(const QModelIndex& index)
{
  if (!index.isValid()) return;
  QString itemText = index.data(Qt::DisplayRole).toString();
  qDebug() << "selected item:" << itemText;
}
