#ifndef PROJECT_TREE_H
#define PROJECT_TREE_H

#include <QContextMenuEvent>
#include <QMenu>
#include <QStandardItemModel>
#include <QTreeView>

#include "tree_model.h"

namespace slicing { class ModelObject; }
namespace csengine { class SceneDocument; }

class ProjectTree : public QTreeView
{
  Q_OBJECT

public:
  explicit ProjectTree(QWidget* parent = nullptr);

  void setSceneDocument(csengine::SceneDocument* doc);

protected:
  void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
  void renameRootItem();
  void addRootNode(slicing::ModelObject* obj);
  void removeModelNode();
  void onItemActivated(const QModelIndex& index);
  void onItemSelected(const QModelIndex& index);

private:
  TreeModel* _model;
  QMenu* _context_menu;
  csengine::SceneDocument* _sceneDocument = nullptr;
};

#endif // PROJECT_TREE_H
