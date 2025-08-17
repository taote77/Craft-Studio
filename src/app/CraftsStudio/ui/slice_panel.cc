#include "slice_panel.h"
#include <QDebug>
#include <QHBoxLayout>

SlicePanel::SlicePanel(QWidget* parent)
  : QWidget(parent)
{
  setupUI();
}

void SlicePanel::setupUI()
{
  _layout = new QHBoxLayout(this);

  // 选择按钮
  // _select_button = new QPushButton("选择模型", this);
  // layout->addWidget(_select_button);

  // _move_button = new QToolButton("移动", this);
  _move_button = new QToolButton(this);
  _move_button->setCheckable(true);
  _move_button->setStyleSheet("QToolButton:checked { background-color: #0078d7; color: white; }");
  _layout->addWidget(_move_button);

  // _rotate_button = new QToolButton("旋转", this);
  _rotate_button = new QToolButton(this);
  _rotate_button->setCheckable(true);
  _rotate_button->setStyleSheet("QToolButton:checked { background-color: #0078d7; color: white; }");
  _layout->addWidget(_rotate_button);

  // _scale_button = new QToolButton("缩放", this);
  _scale_button = new QToolButton(this);
  _scale_button->setCheckable(true);
  _scale_button->setStyleSheet("QToolButton:checked { background-color: #0078d7; color: white; }");
  _layout->addWidget(_scale_button);

  _clear_button = new QPushButton("清除", this);
  _clear_button->setCheckable(true);
  _clear_button->setStyleSheet("QPushButton:checked { background-color: #0078d7; color: white; }");
  _layout->addWidget(_clear_button);

  setLayout(_layout);
}