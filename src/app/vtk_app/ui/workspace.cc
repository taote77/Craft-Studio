#include "workspace.h"

namespace GUI
{

WorkSpace::WorkSpace(QWidget* parent)
  : QMdiArea(parent)
{
  InitUi();
}

void WorkSpace::SetViewMode(QMdiArea::ViewMode mode)
{
  if (mode == QMdiArea::ViewMode::TabbedView)
  {
    this->setViewMode(QMdiArea::ViewMode::TabbedView);
    this->setTabPosition(QTabWidget::TabPosition::South);
    this->setTabsMovable(true);
    this->setTabsClosable(true);
  }
  else
  {
    this->setViewMode(QMdiArea::ViewMode::SubWindowView);
  }
}

void WorkSpace::InitUi()
{
  constexpr int MdiMinWidth = 800;
  constexpr int MdiMinHeight = 600;

  this->setMinimumSize(MdiMinWidth, MdiMinHeight);

  this->SetViewMode(QMdiArea::ViewMode::TabbedView);

  // QPalette palette;

  // // 设置背景颜色为红色
  // palette.setColor(QPalette::Window, Qt::darkMagenta);

  // // 将该调色板应用到QWidget上
  // this->setPalette(palette);

  // // 在设置完调色板后，需要重新绘制界面
  // this->setAutoFillBackground(true);
}

} // namespace GUI
