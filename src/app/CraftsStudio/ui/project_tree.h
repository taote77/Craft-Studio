#ifndef PROJECT_TREE_H
#define PROJECT_TREE_H

#include <QTreeView>

class TreeModel;
class SceneObject;
class QMenu;

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
  TreeModel* _model;
  QMenu* _context_menu;
};

#endif // PROJECT_TREE_H
