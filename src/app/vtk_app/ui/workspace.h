#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QMdiArea>

namespace GUI
{

class WorkSpace : public QMdiArea
{
  Q_OBJECT
public:
  explicit WorkSpace(QWidget* parent = nullptr);

  void SetViewMode(QMdiArea::ViewMode mode);

protected:
  void InitUi();

signals:
};

} // namespace GUI

#endif // WORKSPACE_H
