#ifndef __TRANSFORM_PANEL_H__
#define __TRANSFORM_PANEL_H__

#include <QButtonGroup>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <qobjectdefs.h>

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

signals:
  void modeChanged(EditMode newMode);

private:
  QVBoxLayout* layout;
  QPushButton* _select_button;

  EditModeManager* _editmode_mgr;
  QToolButton* _move_button;
  QToolButton* _rotate_button;
  QToolButton* _scale_button;
  QPushButton* _clear_button;

  QSlider* rotateSlider;
  QLabel* rotateLabel;

  void setupUI();
};

#endif // __TRANSFORM_PANEL_H__