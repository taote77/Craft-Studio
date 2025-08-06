#include "documentview.h"

#include <cmath>
#include <random>
#include <type_traits>

#include <QAction>
#include <QBuffer>
#include <QChar>
#include <QCursor>
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QTemporaryFile>
#include <QVBoxLayout>

#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMemoryResourceStream.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStringArray.h>

#include "rs_interactor_style.h"
#include "rs_interactorstyle_trackcamera.h"

namespace
{
/**
 * Deform the sphere source using a random amplitude and modes and render it in
 * the window
 *
 * @param sphere the original sphere source
 * @param mapper the mapper for the scene
 * @param window the window to render to
 * @param randEng the random number generator engine
 */
void Randomize(vtkSphereSource* sphere, vtkMapper* mapper, vtkGenericOpenGLRenderWindow* window,
  std::mt19937& randEng);
} // namespace

namespace GUI
{

int DocumentView::sIndex = 0;

DocumentView::DocumentView(const QString& win_txt, QWidget* parent)
  : QVTKOpenGLNativeWidget(parent)
  , _vtk_renderer(vtkSmartPointer<vtkRenderer>::New())
{
  Init();
}

vtkSmartPointer<vtkRenderer> DocumentView::GetVtkRenderer() const
{
  return _vtk_renderer;
}

vtkSmartPointer<vtkGenericOpenGLRenderWindow> DocumentView::GetVtkRenderWindow() const
{
  return _render_window;
}

int DocumentView::GetAssinIndex()
{
  sIndex++;
  return DocumentView::sIndex;
}

void DocumentView::Init()
{
  _render_window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

  this->setRenderWindow(_render_window.Get());

  _vtk_renderer->SetBackground(1.0, 1.0, 1.0);
  _vtk_renderer->ResetCamera(); // auto set camera viewport

  _render_window->AddRenderer(_vtk_renderer);

  _interactor = this->interactor();

  auto customStyle = vtkSmartPointer<RSInteractorStyle>::New();

  auto picker = vtkSmartPointer<vtkPropPicker>::New();
  customStyle->SetRenderer(_vtk_renderer);
  customStyle->SetvtkPropPicker(picker);

  _interactor->SetInteractorStyle(customStyle);

  _render_window->Render();

  _right_button_menu = new QMenu(this);

  this->resize(1200, 900);

  QPalette palette;
  palette.setColor(QPalette::Window, Qt::darkMagenta); //    lightGray
  this->setPalette(palette);
  this->setAutoFillBackground(true);

  loadPlatform();
}

void DocumentView::contextMenuEvent(QContextMenuEvent* event)
{
  _right_button_menu->clear();

  auto style_menu = _right_button_menu->addMenu("3D Interactor Style");

  auto action = new QAction("Interact Style");
  connect(action, &QAction::triggered,
    [this]
    {
      auto style = vtkSmartPointer<RSInteractorStyle>::New();

      auto picker = vtkSmartPointer<vtkPropPicker>::New();
      style->SetRenderer(_vtk_renderer);
      style->SetvtkPropPicker(picker);

      _interactor->SetInteractorStyle(style);
    });
  style_menu->addAction(action);

  action = style_menu->addAction("Camera Track");
  connect(action, &QAction::triggered,
    [this]
    {
      auto style = vtkSmartPointer<RSInteractorStyleTrackCamera>::New();

      style->SetRenderer(_vtk_renderer);

      _interactor->SetInteractorStyle(style);
    });

  style_menu->addAction(action);

  _right_button_menu->exec(QCursor::pos());
}

void DocumentView::loadPlatform()
{
  QFile file(":/resources/platform.stl");
  if (!file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Failed to open resource file";
    return;
  }

  QByteArray data = file.readAll();
  file.close();

  auto reader = vtkSmartPointer<vtkSTLReader>::New();

  QTemporaryFile tmp_file;
  // writer file to tmp_file
  tmp_file.open();
  tmp_file.write(data);

  reader->SetFileName(tmp_file.fileName().toStdString().c_str());

  reader->Update();

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(reader->GetOutputPort());

  auto actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  _vtk_renderer->AddActor(actor);
}

} // namespace GUI

namespace
{
void Randomize(vtkSphereSource* sphere, vtkMapper* mapper, vtkGenericOpenGLRenderWindow* window,
  std::mt19937& randEng)
{
  // generate randomness
  double randAmp = 0.2 + ((randEng() % 1000) / 1000.0) * 0.2;
  double randThetaFreq = 1.0 + (randEng() % 9);
  double randPhiFreq = 1.0 + (randEng() % 9);

  // extract and prepare data
  sphere->Update();
  vtkSmartPointer<vtkPolyData> newSphere;
  newSphere.TakeReference(sphere->GetOutput()->NewInstance());
  newSphere->DeepCopy(sphere->GetOutput());
  vtkNew<vtkDoubleArray> height;
  height->SetName("Height");
  height->SetNumberOfComponents(1);
  height->SetNumberOfTuples(newSphere->GetNumberOfPoints());
  newSphere->GetPointData()->AddArray(height);

  // deform the sphere
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

  // reconfigure the pipeline to take the new deformed sphere
  mapper->SetInputDataObject(newSphere);
  mapper->SetScalarModeToUsePointData();
  mapper->ColorByArrayComponent("Height", 0);
  window->Render();
}
} // namespace
