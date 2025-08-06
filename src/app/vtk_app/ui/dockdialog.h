#ifndef DOCKDIALOG_H
#define DOCKDIALOG_H

#include <QDockWidget>
#include <QTabWidget>
#include <qboxlayout.h>

namespace GUI
{

class DockDialog : public QDockWidget
{
  Q_OBJECT
public:
  explicit DockDialog(QWidget* parent = nullptr);

protected:
  void InitUI();

signals:

private:
  QTabWidget* _table_widget;
};

} // namespace GUI

#endif // DOCKDIALOG_H
