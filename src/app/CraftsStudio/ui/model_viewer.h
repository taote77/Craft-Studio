#ifndef _MODEL_VIEWER_H
#define _MODEL_VIEWER_H

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSmartPointer.h>

class ModelViewer : public QVTKOpenGLNativeWidget
{
  Q_OBJECT
public:
  ModelViewer(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
};

#endif // _MODEL_VIEWER_H
