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

namespace slicing { class ModelObject; }
namespace csengine { class SceneDocument; }

// 编辑模式枚举
enum class EditMode
{
  None,
  Translate,
  Rotate,
  Scale
};

class EditModeManager : public QObject
{
  Q_OBJECT
public:
  explicit EditModeManager(QObject* parent = nullptr);
  void initButtons(QToolButton* translateBtn, QToolButton* rotateBtn, QToolButton* scaleBtn);
  void setCurrentMode(EditMode mode);
  EditMode currentMode() const { return _current_mode; }
  void clearSelection();

signals:
  void modeChanged(EditMode newMode);

private slots:
  void onButtonClicked(int id);

private:
  EditMode _current_mode = EditMode::None;
  QButtonGroup* _button_group = nullptr;
};

class TransformPanel : public QWidget
{
  Q_OBJECT

public:
  explicit TransformPanel(QWidget* parent = nullptr);

  // Set scene document (replaces old SceneManager)
  void setSceneDocument(csengine::SceneDocument* doc);

  // Update selected object info
  void updateSelection(slicing::ModelObject* selectedObject);

signals:
  void modeChanged(EditMode newMode);
  void transformApplied();

public slots:
  void onTranslationChanged();
  void onRotationChanged();
  void onScaleChanged();

private slots:
  void onSelectionChanged();
  void onTransformModeChanged(int mode);

private:
  QVBoxLayout* layout = nullptr;
  csengine::SceneDocument* _sceneDocument = nullptr;

  EditModeManager* _editmode_mgr;
  QToolButton* _move_button;
  QToolButton* _rotate_button;
  QToolButton* _scale_button;
  QPushButton* _clear_button;

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

  slicing::ModelObject* m_currentObject = nullptr;
  bool m_updating = false;

  void setupUI();
  void createTransformInputs();
  void updateTransformControls();
  void enableTransformControls(bool enabled);
};

#endif // __TRANSFORM_PANEL_H__
