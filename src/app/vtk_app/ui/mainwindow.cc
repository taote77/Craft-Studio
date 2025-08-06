#include "mainwindow.h"
#include "menu_manager.h"

#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkProperty.h>

#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setupUI();
}

void MainWindow::setupUI()
{
  this->resize(1200, 900);

  _action_factory = new ActionFactory(this);

  connect(_action_factory, &ActionFactory::sigExit, this, &MainWindow::exit);

  auto menu_mgr = new MenuManager(this);
  menu_mgr->init(*this, *_action_factory);

  // dock
  addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

  dockLayout = new QVBoxLayout();
  layoutContainer.setLayout(dockLayout);
  controlDock.setWidget(&layoutContainer);

  randomizeButton.setText("Randomize");
  dockLayout->addWidget(&randomizeButton);

  // Render area.
  _vtkRenderWidget = new QVTKOpenGLNativeWidget();
  this->setCentralWidget(_vtkRenderWidget);

  // VTK part.
  _vtkRenderWindow = vtkNew<vtkGenericOpenGLRenderWindow>();
  _vtkRenderWidget->setRenderWindow(_vtkRenderWindow);

  _action_factory->setVtkRenderWindow(_vtkRenderWindow);

  _sphere = vtkNew<vtkSphereSource>();
  _sphere->SetRadius(1.0);
  _sphere->SetThetaResolution(100);
  _sphere->SetPhiResolution(100);

  _mapper = vtkNew<vtkDataSetMapper>();
  _mapper->SetInputConnection(_sphere->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(_mapper);
  actor->GetProperty()->SetEdgeVisibility(true);
  actor->GetProperty()->SetRepresentationToSurface();

  _vtkRenderer = vtkNew<vtkRenderer>();

  _action_factory->setVtkRenderer(_vtkRenderer);
  _vtkRenderer->AddActor(actor);

  _vtkRenderWindow->AddRenderer(_vtkRenderer);
}

/**
 * Deform the sphere source using a random amplitude and modes and render it in
 * the window
 *
 * @param sphere the original sphere source
 * @param mapper the mapper for the scene
 * @param window the window to render to
 * @param randEng the random number generator engine
 */
void MainWindow::Randomize(vtkSphereSource* sphere, vtkDataSetMapper* mapper,
  vtkGenericOpenGLRenderWindow* window, std::mt19937& randEng)
{
  // Generate randomness.
  double randAmp = 0.2 + ((randEng() % 1000) / 1000.0) * 0.2;
  double randThetaFreq = 1.0 + (randEng() % 9);
  double randPhiFreq = 1.0 + (randEng() % 9);

  // Extract and prepare data.
  sphere->Update();
  vtkSmartPointer<vtkPolyData> newSphere;
  newSphere.TakeReference(sphere->GetOutput()->NewInstance());
  newSphere->DeepCopy(sphere->GetOutput());
  vtkNew<vtkDoubleArray> height;
  height->SetName("Height");
  height->SetNumberOfComponents(1);
  height->SetNumberOfTuples(newSphere->GetNumberOfPoints());
  newSphere->GetPointData()->AddArray(height);

  // Deform the sphere.
  for (int iP = 0; iP < newSphere->GetNumberOfPoints(); iP++)
  {
    double pt[3] = { 0.0 };
    newSphere->GetPoint(iP, pt);
    double theta = std::atan2(pt[1], pt[0]);
    double phi = std::atan2(pt[2], std::sqrt(std::pow(pt[0], 2) + std::pow(pt[1], 2)));
    double thisAmp = randAmp * std::cos(randThetaFreq * theta) * std::sin(randPhiFreq * phi);
    height->SetValue(iP, thisAmp);
    pt[0] += thisAmp * std::cos(theta) * std::cos(phi);
    pt[1] += thisAmp * std::sin(theta) * std::cos(phi);
    pt[2] += thisAmp * std::sin(phi);
    newSphere->GetPoints()->SetPoint(iP, pt);
  }
  newSphere->GetPointData()->SetScalars(height);

  // Reconfigure the pipeline to take the new deformed sphere.
  mapper->SetInputDataObject(newSphere);
  mapper->SetScalarModeToUsePointData();
  mapper->ColorByArrayComponent("Height", 0);
  window->Render();
}

void MainWindow::exit()
{
  qDebug() << "exiout";

  QApplication::exit();
}
