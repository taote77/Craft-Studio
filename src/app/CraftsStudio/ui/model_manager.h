#ifndef _MODEL_MANAGER_H
#define _MODEL_MANAGER_H

// tree widget
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <qtreeview.h>

class ModelManager : public QTreeView
{
  Q_OBJECT
public:
  ModelManager(QWidget* parent = nullptr);

private:
};

#endif // _MODEL_MANAGER_H