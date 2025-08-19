#include "model_manager.h"

#include <QAction>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QInputDialog>
#include <QMenu>
#include <QMimeData>

ModelManager::ModelManager(QWidget* parent)
  : QTreeView(parent)
{
  // init model
}
