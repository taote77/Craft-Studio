#ifndef PROJECT_TREE_H
#define PROJECT_TREE_H

#include <QDockWidget>
#include <QVBoxLayout>

class ProjectTree : public QDockWidget
{
  Q_OBJECT
public:
  explicit ProjectTree(QWidget* parent = nullptr);

  void addWidget(QWidget* widget, int stretch = 0, Qt::Alignment alignment = Qt::Alignment());

protected:
  void InitUi();

private:
  QVBoxLayout* _vbox_layout;

  QWidget* _container_widget;
};

#endif // PROJECT_TREE_H