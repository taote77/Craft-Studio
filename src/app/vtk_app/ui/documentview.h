#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QPointer>
#include <QPushButton>
#include <QVTKOpenGLNativeWidget.h>
#include <QWidget>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

class QTextEdit;
class QVBoxLayout;

namespace GUI
{

class DocumentView final : public QVTKOpenGLNativeWidget
{
public:
  explicit DocumentView(const QString& win_txt, QWidget* parent = nullptr);

  vtkSmartPointer<vtkRenderer> GetVtkRenderer() const;

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> GetVtkRenderWindow() const;

  static int GetAssinIndex();

protected:
  void Init();

  void contextMenuEvent(QContextMenuEvent* event) override;

private:
  void loadPlatform();

private:
  static int sIndex;

  vtkSmartPointer<vtkRenderer> _vtk_renderer;

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _render_window;

  vtkSmartPointer<vtkRenderWindowInteractor> _interactor;

  // 定义菜单
  QMenu* _right_button_menu{ nullptr }; // 右键菜单
};

} // namespace GUI

#endif // DOCUMENTVIEW_H
