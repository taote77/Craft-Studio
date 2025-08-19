#ifndef __SLICE_PANEL_H__
#define __SLICE_PANEL_H__

#include <QButtonGroup>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <qobjectdefs.h>

class SlicePanel : public QWidget
{
  Q_OBJECT

public:
  explicit SlicePanel(QWidget* parent = nullptr);

signals:
  // void modeChanged(EditMode newMode);

private:
  QHBoxLayout* _layout;
  QPushButton* _select_button;

  QToolButton* _move_button;
  QToolButton* _rotate_button;
  QToolButton* _scale_button;
  QPushButton* _clear_button;

  QSlider* rotateSlider;
  QLabel* rotateLabel;

  void setupUI();
};

#endif // __SLICE_PANEL_H__
