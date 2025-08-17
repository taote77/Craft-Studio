#ifndef PROJECT_TREE_H
#define PROJECT_TREE_H

#include <QContextMenuEvent>
#include <QMenu>
#include <QStandardItemModel>
#include <QTreeView>

#include "engine/rs_scene_object.h"
#include "tree_model.h"

class ProjectTree : public QTreeView
{
  Q_OBJECT

public:
  explicit ProjectTree(QWidget* parent = nullptr);

protected:
  void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
  void renameRootItem();

  void addRootNode(SceneObject* obj);

  void removeNode(SceneObject* obj);

  void onItemActivated(const QModelIndex& index);

  void onItemSelected(const QModelIndex& index);

private:
  // QStandardItemModel* _model;
  TreeModel* _model;
  QMenu* _context_menu;
};

#endif // PROJECT_TREE_H
