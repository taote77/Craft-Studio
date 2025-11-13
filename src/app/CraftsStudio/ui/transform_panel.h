#ifndef __TRANSFORM_PANEL_H__
#define __TRANSFORM_PANEL_H__

#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <qobjectdefs.h>

#include "engine/rs_scene_manager.h"

// 编辑模式枚举
enum class EditMode
{
  None,      // 无模式
  Translate, // 移动模式
  Rotate,    // 旋转模式
  Scale      // 缩放模式
};

class EditModeManager : public QObject
{
  Q_OBJECT
public:
  explicit EditModeManager(QObject* parent = nullptr);

  // 初始化模式按钮
  void initButtons(QToolButton* translateBtn, QToolButton* rotateBtn, QToolButton* scaleBtn);

  // 设置当前模式
  void setCurrentMode(EditMode mode);

  // 获取当前模式
  EditMode currentMode() const { return _current_mode; }

  // 取消所有模式选择
  void clearSelection();

signals:
  // 模式改变信号
  void modeChanged(EditMode newMode);

private slots:
  // 按钮点击处理
  void onButtonClicked(int id);

private:
  EditMode _current_mode;      // 当前模式
  QButtonGroup* _button_group; // 按钮组
};

class TransformPanel : public QWidget
{
  Q_OBJECT

public:
  explicit TransformPanel(QWidget* parent = nullptr);
  
  // 设置场景管理器
  void setSceneManager(SceneManager* manager);
  
  // 更新选中对象信息
  void updateSelection(SceneObject* selectedObject);

signals:
  void modeChanged(EditMode newMode);
  void transformApplied();

public slots:
  // 处理变换操作
  void onTranslationChanged();
  void onRotationChanged();
  void onScaleChanged();

private slots:
  // 处理场景管理器信号
  void onSelectionChanged(SceneObject* selectedObject);
  void onTransformModeChanged(TransformMode mode);

private:
  QVBoxLayout* layout;
  
  // 场景管理器
  SceneManager* m_sceneManager = nullptr;
  
  // 模式管理
  EditModeManager* _editmode_mgr;
  QToolButton* _move_button;
  QToolButton* _rotate_button;
  QToolButton* _scale_button;
  QPushButton* _clear_button;
  
  // 变换数值输入
  QLabel* m_positionLabel;
  QLineEdit* m_positionXEdit;
  QLineEdit* m_positionYEdit;
  QLineEdit* m_positionZEdit;
  
  QLabel* m_rotationLabel;
  QLineEdit* m_rotationXEdit;
  QLineEdit* m_rotationYEdit;
  QLineEdit* m_rotationZEdit;
  
  QLabel* m_scaleLabel;
  QLineEdit* m_scaleXEdit;
  QLineEdit* m_scaleYEdit;
  QLineEdit* m_scaleZEdit;
  
  // 当前选中的对象
  SceneObject* m_currentObject = nullptr;
  
  // 防止递归更新
  bool m_updating = false;

  void setupUI();
  void createTransformInputs();
  void updateTransformControls();
  void enableTransformControls(bool enabled);
};

#endif // __TRANSFORM_PANEL_H__