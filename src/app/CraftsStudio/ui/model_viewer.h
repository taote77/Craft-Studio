#ifndef _MODEL_VIEWER_H
#define _MODEL_VIEWER_H

#include <QVTKOpenGLNativeWidget.h>

#include <vtkCameraOrientationWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderer.h>
#include <vtkSliderWidget.h>
#include <vtkSmartPointer.h>

class ModelViewer : public QVTKOpenGLNativeWidget {
  Q_OBJECT
public:
  ModelViewer(QWidget *parent = nullptr,
              Qt::WindowFlags flag = Qt::WindowFlags());

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> getRenderWindow();

  vtkSmartPointer<vtkRenderer> getVtkRenderer();

protected:
  void setUp();

private:
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _render_window{nullptr};

  vtkSmartPointer<vtkRenderer> _vtk_renderer{nullptr};

  vtkSmartPointer<vtkOrientationMarkerWidget> _axes_widget{nullptr};

  vtkSmartPointer<vtkCameraOrientationWidget> _cam_widget{nullptr};
};

#endif // _MODEL_VIEWER_H
