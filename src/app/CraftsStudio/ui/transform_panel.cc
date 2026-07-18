#include "transform_panel.h"
#include <csengine/scene/scene_document.hpp>
#include <slicingcore/mesh/model_object.hpp>
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

  // 创建变换数值输入框
  createTransformInputs();

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

  setLayout(layout);
}

void TransformPanel::createTransformInputs()
{
  // 位置输入框
  m_positionLabel = new QLabel("位置", this);
  layout->addWidget(m_positionLabel);
  
  QHBoxLayout* posLayout = new QHBoxLayout();
  m_positionXEdit = new QLineEdit(this);
  m_positionYEdit = new QLineEdit(this);
  m_positionZEdit = new QLineEdit(this);
  
  m_positionXEdit->setPlaceholderText("X");
  m_positionYEdit->setPlaceholderText("Y");
  m_positionZEdit->setPlaceholderText("Z");
  
  posLayout->addWidget(m_positionXEdit);
  posLayout->addWidget(m_positionYEdit);
  posLayout->addWidget(m_positionZEdit);
  layout->addLayout(posLayout);
  
  // 连接位置输入框信号
  connect(m_positionXEdit, &QLineEdit::textChanged, this, &TransformPanel::onTranslationChanged);
  connect(m_positionYEdit, &QLineEdit::textChanged, this, &TransformPanel::onTranslationChanged);
  connect(m_positionZEdit, &QLineEdit::textChanged, this, &TransformPanel::onTranslationChanged);

  // 旋转输入框
  m_rotationLabel = new QLabel("旋转", this);
  layout->addWidget(m_rotationLabel);
  
  QHBoxLayout* rotLayout = new QHBoxLayout();
  m_rotationXEdit = new QLineEdit(this);
  m_rotationYEdit = new QLineEdit(this);
  m_rotationZEdit = new QLineEdit(this);
  
  m_rotationXEdit->setPlaceholderText("X");
  m_rotationYEdit->setPlaceholderText("Y");
  m_rotationZEdit->setPlaceholderText("Z");
  
  rotLayout->addWidget(m_rotationXEdit);
  rotLayout->addWidget(m_rotationYEdit);
  rotLayout->addWidget(m_rotationZEdit);
  layout->addLayout(rotLayout);
  
  // 连接旋转输入框信号
  connect(m_rotationXEdit, &QLineEdit::textChanged, this, &TransformPanel::onRotationChanged);
  connect(m_rotationYEdit, &QLineEdit::textChanged, this, &TransformPanel::onRotationChanged);
  connect(m_rotationZEdit, &QLineEdit::textChanged, this, &TransformPanel::onRotationChanged);

  // 缩放输入框
  m_scaleLabel = new QLabel("缩放", this);
  layout->addWidget(m_scaleLabel);
  
  QHBoxLayout* scaleLayout = new QHBoxLayout();
  m_scaleXEdit = new QLineEdit(this);
  m_scaleYEdit = new QLineEdit(this);
  m_scaleZEdit = new QLineEdit(this);
  
  m_scaleXEdit->setPlaceholderText("X");
  m_scaleYEdit->setPlaceholderText("Y");
  m_scaleZEdit->setPlaceholderText("Z");
  
  scaleLayout->addWidget(m_scaleXEdit);
  scaleLayout->addWidget(m_scaleYEdit);
  scaleLayout->addWidget(m_scaleZEdit);
  layout->addLayout(scaleLayout);
  
  // 连接缩放输入框信号
  connect(m_scaleXEdit, &QLineEdit::textChanged, this, &TransformPanel::onScaleChanged);
  connect(m_scaleYEdit, &QLineEdit::textChanged, this, &TransformPanel::onScaleChanged);
  connect(m_scaleZEdit, &QLineEdit::textChanged, this, &TransformPanel::onScaleChanged);
  
  // 初始时禁用输入框
  enableTransformControls(false);
}

void TransformPanel::setSceneDocument(csengine::SceneDocument* doc)
{
  _sceneDocument = doc;

  if (_sceneDocument)
  {
    connect(_sceneDocument, &csengine::SceneDocument::selectionChanged,
            this, &TransformPanel::onSelectionChanged);
  }
}

void TransformPanel::updateSelection(slicing::ModelObject* selectedObject)
{
  m_currentObject = selectedObject;
  updateTransformControls();
}

void TransformPanel::onTranslationChanged()
{
  if (m_updating || !m_currentObject || !_sceneDocument)
    return;

  m_updating = true;
  
  // 获取输入框中的变换值
  double x = m_positionXEdit->text().toDouble();
  double y = m_positionYEdit->text().toDouble();
  double z = m_positionZEdit->text().toDouble();
  
  // 应用变换到当前对象
  _sceneDocument->translate({m_currentObject}, x, y, z);
  
  m_updating = false;
  emit transformApplied();
}

void TransformPanel::onRotationChanged()
{
  if (m_updating || !m_currentObject || !_sceneDocument)
    return;

  m_updating = true;
  
  // 获取输入框中的旋转值
  double x = m_rotationXEdit->text().toDouble();
  double y = m_rotationYEdit->text().toDouble();
  double z = m_rotationZEdit->text().toDouble();
  
  // 应用旋转到当前对象
  _sceneDocument->rotate({m_currentObject}, x, y, z);
  
  m_updating = false;
  emit transformApplied();
}

void TransformPanel::onScaleChanged()
{
  if (m_updating || !m_currentObject || !_sceneDocument)
    return;

  m_updating = true;
  
  // 获取输入框中的缩放值
  double x = m_scaleXEdit->text().toDouble();
  double y = m_scaleYEdit->text().toDouble();
  double z = m_scaleZEdit->text().toDouble();
  
  // 应用缩放到当前对象
  _sceneDocument->scale({m_currentObject}, x, y, z);
  
  m_updating = false;
  emit transformApplied();
}

void TransformPanel::onSelectionChanged()
{
  // Update from the first selected object (TODO: wire properly when used)
  if (_sceneDocument && !_sceneDocument->selection().isEmpty())
    updateSelection(_sceneDocument->selection().first());
}

void TransformPanel::onTransformModeChanged(int mode)
{
  // 根据变换模式更新UI状态
  // 这里可以根据需要实现模式切换的逻辑
}

void TransformPanel::updateTransformControls()
{
  if (!m_currentObject)
  {
    enableTransformControls(false);
    return;
  }

  enableTransformControls(true);

  // Read from ModelObject placement
  const auto& p = m_currentObject->placement();
  m_positionXEdit->setText(QString::number(p.posX, 'f', 2));
  m_positionYEdit->setText(QString::number(p.posY, 'f', 2));
  m_positionZEdit->setText(QString::number(p.posZ, 'f', 2));

  m_rotationXEdit->setText(QString::number(p.rotX, 'f', 2));
  m_rotationYEdit->setText(QString::number(p.rotY, 'f', 2));
  m_rotationZEdit->setText(QString::number(p.rotZ, 'f', 2));

  m_scaleXEdit->setText(QString::number(p.scaleX, 'f', 2));
  m_scaleYEdit->setText(QString::number(p.scaleY, 'f', 2));
  m_scaleZEdit->setText(QString::number(p.scaleZ, 'f', 2));
}

void TransformPanel::enableTransformControls(bool enabled)
{
  m_positionXEdit->setEnabled(enabled);
  m_positionYEdit->setEnabled(enabled);
  m_positionZEdit->setEnabled(enabled);
  
  m_rotationXEdit->setEnabled(enabled);
  m_rotationYEdit->setEnabled(enabled);
  m_rotationZEdit->setEnabled(enabled);
  
  m_scaleXEdit->setEnabled(enabled);
  m_scaleYEdit->setEnabled(enabled);
  m_scaleZEdit->setEnabled(enabled);
}
