#include "transform_panel.h"
#include <QDebug>
#include <QHBoxLayout>
#include <qglobal.h>
#include <qobjectdefs.h>

EditModeManager::EditModeManager(QObject* parent)
  : QObject(parent)
  , _current_mode(EditMode::None)
{
  // 创建按钮组，不设置互斥性，因为我们需要支持取消选择
  _button_group = new QButtonGroup(this);
  _button_group->setExclusive(false);

  // 连接按钮点击信号

#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
  connect(_button_group, QOverload<int>::of(&QButtonGroup::idClicked), this,
    &EditModeManager::onButtonClicked);
#else
  connect(_button_group, QOverload<int>::of(&QButtonGroup::buttonClicked), this,
    &EditModeManager::onButtonClicked);
#endif






}

void EditModeManager::initButtons(
  QToolButton* translateBtn, QToolButton* rotateBtn, QToolButton* scaleBtn)
{
  if (!translateBtn || !rotateBtn || !scaleBtn)
    return;

  // 设置按钮为可选中状态
  translateBtn->setCheckable(true);
  rotateBtn->setCheckable(true);
  scaleBtn->setCheckable(true);

  // 设置按钮图标（这里使用文本代替，实际项目中应使用QIcon）
  translateBtn->setText("移动");
  rotateBtn->setText("旋转");
  scaleBtn->setText("缩放");

  // 将按钮添加到按钮组，并分配ID
  _button_group->addButton(translateBtn, static_cast<int>(EditMode::Translate));
  _button_group->addButton(rotateBtn, static_cast<int>(EditMode::Rotate));
  _button_group->addButton(scaleBtn, static_cast<int>(EditMode::Scale));
}

void EditModeManager::setCurrentMode(EditMode mode)
{
  if (_current_mode == mode)
    return;

  // 取消之前选中的按钮
  if (_current_mode != EditMode::None)
  {
    QAbstractButton* prevButton = _button_group->button(static_cast<int>(_current_mode));
    if (prevButton)
      prevButton->setChecked(false);
  }

  // 选中新的按钮
  _current_mode = mode;
  if (_current_mode != EditMode::None)
  {
    QAbstractButton* newButton = _button_group->button(static_cast<int>(_current_mode));
    if (newButton)
    {
      newButton->setChecked(true);
    }
  }

  // 发射模式改变信号
  emit modeChanged(_current_mode);
}

void EditModeManager::clearSelection()
{
  setCurrentMode(EditMode::None);
}

void EditModeManager::onButtonClicked(int id)
{
  // 获取点击的按钮
  QAbstractButton* clickedButton = _button_group->button(id);
  if (!clickedButton)
    return;

  // 转换ID为模式
  EditMode clickedMode = static_cast<EditMode>(id);

  // 如果点击的按钮已被选中，则取消选择（进入无模式）
  if (clickedButton->isChecked() && _current_mode == clickedMode)
  {
    clearSelection();
  }
  // 否则切换到该模式
  else
  {
    setCurrentMode(clickedMode);
  }
}

TransformPanel::TransformPanel(QWidget* parent)
  : QWidget(parent)
{
  setupUI();
}

void TransformPanel::setupUI()
{
  layout = new QVBoxLayout(this);

  // 选择按钮
  // _select_button = new QPushButton("选择模型", this);
  // layout->addWidget(_select_button);

  // _move_button = new QToolButton("移动", this);
  _move_button = new QToolButton(this);
  _move_button->setCheckable(true);
  _move_button->setStyleSheet("QToolButton:checked { background-color: #0078d7; color: white; }");
  layout->addWidget(_move_button);

  // _rotate_button = new QToolButton("旋转", this);
  _rotate_button = new QToolButton(this);
  _rotate_button->setCheckable(true);
  _rotate_button->setStyleSheet("QToolButton:checked { background-color: #0078d7; color: white; }");
  layout->addWidget(_rotate_button);

  // _scale_button = new QToolButton("缩放", this);
  _scale_button = new QToolButton(this);
  _scale_button->setCheckable(true);
  _scale_button->setStyleSheet("QToolButton:checked { background-color: #0078d7; color: white; }");
  layout->addWidget(_scale_button);

  _clear_button = new QPushButton("清除", this);
  _clear_button->setCheckable(true);
  _clear_button->setStyleSheet("QPushButton:checked { background-color: #0078d7; color: white; }");
  layout->addWidget(_clear_button);

  _editmode_mgr = new EditModeManager(this);

  _editmode_mgr->initButtons(_move_button, _rotate_button, _scale_button);

  // 连接模式变化信号
  connect(_editmode_mgr, &EditModeManager::modeChanged, this,
    [this](EditMode mode)
    {
      QString modeText;
      switch (mode)
      {
        case EditMode::None:
          modeText = "无";
          break;
        case EditMode::Translate:
          modeText = "移动";
          break;
        case EditMode::Rotate:
          modeText = "旋转";
          break;
        case EditMode::Scale:
          modeText = "缩放";
          break;
      }
      // qDebug() << "当前模式:" << modeText << (int)mode;

      Q_EMIT modeChanged(mode);
    });

  // 连接清除选择按钮
  connect(_clear_button, &QToolButton::clicked, _editmode_mgr, &EditModeManager::clearSelection);

  // // 旋转控件
  // rotateLabel = new QLabel("旋转", this);
  // rotateLabel->setStyleSheet("QLabel { background-color: #0078d7; color: white; }");

  // layout->addWidget(rotateLabel);
  // rotateSlider = new QSlider(Qt::Horizontal, this);
  // rotateSlider->setRange(0, 360);
  // layout->addWidget(rotateSlider);

  setLayout(layout);
}
