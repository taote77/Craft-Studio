#include <csengine/scene/scene_document.hpp>
#include <csengine/scene/vtk_converter.hpp>
#include <csengine/command/command.hpp>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include <QDebug>

namespace csengine {

// Default colors (VTK 9.6 SetColor takes non-const double*)
static double DEFAULT_COLOR[] = { 0.7, 0.7, 0.7 };
static double SELECTED_COLOR[] = { 1.0, 0.8, 0.0 }; // yellow

// ============================================================================
// SceneDocument
// ============================================================================

SceneDocument::SceneDocument(QObject* parent)
    : QObject(parent)
    , _commandStack(std::make_unique<CommandStack>())
{
    connect(_commandStack.get(), &CommandStack::stateChanged,
            this, &SceneDocument::commandStateChanged);
}

SceneDocument::~SceneDocument()
{
    // Clear VTK actors
    for (auto* obj : _objects)
        removeActor(obj);

    // Delete all model objects
    qDeleteAll(_objects);
    _objects.clear();
}

// -- Model Management --------------------------------------------------------

slicing::ModelObject* SceneDocument::addModel(slicing::TriangleMesh mesh,
                                               const QString& name)
{
    auto* cmd = new AddModelCommand(this, std::move(mesh), name);
    _commandStack->execute(cmd);
    return _objects.last();
}

slicing::ModelObject* SceneDocument::addModelWithVTK(slicing::TriangleMesh mesh,
                                                      vtkPolyData* polyData,
                                                      const QString& name)
{
    auto* obj = new slicing::ModelObject(name.toStdString());
    obj->addVolume(std::move(mesh), name.toStdString());

    // Use the provided VTK polydata for rendering (no re-conversion)
    createActorFromVTK(obj, polyData);

    internalAddObject(obj);
    return obj;
}

void SceneDocument::removeModels(const QList<slicing::ModelObject*>& objects)
{
    if (objects.isEmpty()) return;

    auto* cmd = new RemoveModelCommand(this, objects);
    _commandStack->execute(cmd);
    emit objectsRemoved();
}

void SceneDocument::clearAll()
{
    if (_objects.isEmpty()) return;

    emit objectsAboutToBeRemoved(_objects);
    QList<slicing::ModelObject*> toRemove = _objects;

    for (auto* obj : toRemove)
        internalRemoveObject(obj);

    _selection.clear();
    _commandStack->clear();

    emit objectsRemoved();
}

const QList<slicing::ModelObject*>& SceneDocument::objects() const
{
    return _objects;
}

slicing::ModelObject* SceneDocument::findObject(uint64_t id) const
{
    for (auto* obj : _objects)
    {
        if (obj->id() == id)
            return obj;
    }
    return nullptr;
}

slicing::ModelObject* SceneDocument::findObjectByActor(vtkActor* actor) const
{
    auto it = _actorToObjectId.find(actor);
    if (it != _actorToObjectId.end())
        return findObject(it.value());
    return nullptr;
}

// -- Selection ---------------------------------------------------------------

QList<slicing::ModelObject*> SceneDocument::selection() const
{
    return _selection;
}

void SceneDocument::setSelected(slicing::ModelObject* obj, bool additive)
{
    if (!additive)
        clearSelection();

    if (obj && !_selection.contains(obj))
    {
        _selection.append(obj);
        syncSelectionAppearance();
        emit selectionChanged(_selection);
    }
}

void SceneDocument::addToSelection(slicing::ModelObject* obj)
{
    setSelected(obj, true);
}

void SceneDocument::removeFromSelection(slicing::ModelObject* obj)
{
    _selection.removeOne(obj);
    syncSelectionAppearance();
    emit selectionChanged(_selection);
}

void SceneDocument::clearSelection()
{
    if (_selection.isEmpty()) return;
    _selection.clear();
    syncSelectionAppearance();
    emit selectionChanged(_selection);
}

void SceneDocument::selectAll()
{
    _selection = _objects;
    syncSelectionAppearance();
    emit selectionChanged(_selection);
}

bool SceneDocument::hasSelection() const
{
    return !_selection.isEmpty();
}

// -- Transform ---------------------------------------------------------------

void SceneDocument::translate(const QList<slicing::ModelObject*>& objects,
                               double dx, double dy, double dz)
{
    if (objects.isEmpty()) return;
    auto* cmd = new TranslateCommand(this, objects, dx, dy, dz);
    _commandStack->execute(cmd);
    for (auto* obj : objects)
        emit objectModified(obj);
}

void SceneDocument::rotate(const QList<slicing::ModelObject*>& objects,
                            double rx, double ry, double rz)
{
    if (objects.isEmpty()) return;
    auto* cmd = new RotateCommand(this, objects, rx, ry, rz);
    _commandStack->execute(cmd);
    for (auto* obj : objects)
        emit objectModified(obj);
}

void SceneDocument::scale(const QList<slicing::ModelObject*>& objects,
                           double sx, double sy, double sz)
{
    if (objects.isEmpty()) return;
    for (auto* obj : objects)
    {
        auto oldPlacement = obj->placement();
        obj->setScale(
            oldPlacement.scaleX * sx,
            oldPlacement.scaleY * sy,
            oldPlacement.scaleZ * sz
        );
        updateActorTransform(obj);
        emit objectModified(obj);
    }
}

// -- Collision Detection -------------------------------------------------------

QList<SceneDocument::Collision> SceneDocument::checkCollisions() const
{
    QList<Collision> collisions;

    for (int i = 0; i < _objects.size(); ++i)
    {
        for (int j = i + 1; j < _objects.size(); ++j)
        {
            auto* a = _objects[i];
            auto* b = _objects[j];

            double aMinX, aMinY, aMinZ, aMaxX, aMaxY, aMaxZ;
            double bMinX, bMinY, bMinZ, bMaxX, bMaxY, bMaxZ;
            a->worldBounds(aMinX, aMinY, aMinZ, aMaxX, aMaxY, aMaxZ);
            b->worldBounds(bMinX, bMinY, bMinZ, bMaxX, bMaxY, bMaxZ);

            // 2D XY overlap check (models must not share the same space)
            bool overlapX = aMaxX > bMinX && bMaxX > aMinX;
            bool overlapY = aMaxY > bMinY && bMaxY > aMinY;

            if (overlapX && overlapY)
                collisions.append(Collision{ a, b });
        }
    }

    return collisions;
}

// -- VTK Synchronization ------------------------------------------------------

void SceneDocument::setRenderer(vtkRenderer* renderer)
{
    _renderer = renderer;
}

void SceneDocument::syncToRenderer()
{
    if (!_renderer) return;

    for (auto* obj : _objects)
    {
        vtkSmartPointer<vtkActor> actor = _objectToActor.value(obj->id());
        if (actor)
        {
            updateActorTransform(obj);
            _renderer->AddActor(actor);
        }
    }
}

void SceneDocument::syncSelectionAppearance()
{
    for (auto* obj : _objects)
    {
        vtkSmartPointer<vtkActor> actor = _objectToActor.value(obj->id());
        if (!actor) continue;

        bool selected = _selection.contains(obj);
        if (selected)
            actor->GetProperty()->SetColor(SELECTED_COLOR);
        else
            actor->GetProperty()->SetColor(DEFAULT_COLOR);
    }
}

vtkActor* SceneDocument::actorForObject(slicing::ModelObject* obj)
{
    auto it = _objectToActor.find(obj->id());
    if (it != _objectToActor.end())
        return it.value().Get();

    createActor(obj);
    vtkSmartPointer<vtkActor> actor = _objectToActor.value(obj->id());
    return actor.Get();
}

// -- Internal Methods --------------------------------------------------------

void SceneDocument::internalAddObject(slicing::ModelObject* obj)
{
    _objects.append(obj);

    // Only create actor if one doesn't already exist (addModelWithVTK pre-creates)
    if (!_objectToActor.contains(obj->id()))
        createActor(obj);

    if (_renderer && _objectToActor.contains(obj->id()))
        _renderer->AddActor(_objectToActor[obj->id()]);

    emit objectsAdded({ obj });
}

void SceneDocument::internalRemoveObject(slicing::ModelObject* obj)
{
    _selection.removeOne(obj);
    removeActor(obj);
    _objects.removeOne(obj);
    delete obj;
}

void SceneDocument::createActor(slicing::ModelObject* obj)
{
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetColor(DEFAULT_COLOR);
    actor->GetProperty()->SetOpacity(1.0);
    actor->GetProperty()->SetInterpolationToPhong();

    // Build geometry from the first volume's mesh
    if (obj->volumeCount() > 0)
    {
        const auto& mesh = obj->volumes()[0].mesh();
        if (mesh.triangleCount() > 0)
        {
            vtkPolyData* polyData = meshToVTK(mesh);
            vtkNew<vtkPolyDataMapper> mapper;
            mapper->SetInputData(polyData);
            polyData->Delete(); // mapper holds a reference
            actor->SetMapper(mapper);
        }
    }

    // Set initial position from placement
    const auto& p = obj->placement();
    actor->SetPosition(p.posX, p.posY, p.posZ);
    actor->SetOrientation(p.rotX, p.rotY, p.rotZ);
    actor->SetScale(p.scaleX, p.scaleY, p.scaleZ);

    _objectToActor.insert(obj->id(), actor);
    _actorToObjectId.insert(actor.Get(), obj->id());
}

void SceneDocument::createActorFromVTK(slicing::ModelObject* obj, vtkPolyData* polyData)
{
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetColor(DEFAULT_COLOR);
    actor->GetProperty()->SetOpacity(1.0);
    actor->GetProperty()->SetInterpolationToPhong();

    if (polyData)
    {
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor->SetMapper(mapper);
    }

    const auto& p = obj->placement();
    actor->SetPosition(p.posX, p.posY, p.posZ);
    actor->SetOrientation(p.rotX, p.rotY, p.rotZ);
    actor->SetScale(p.scaleX, p.scaleY, p.scaleZ);

    _objectToActor.insert(obj->id(), actor);
    _actorToObjectId.insert(actor.Get(), obj->id());
}

void SceneDocument::updateActorTransform(slicing::ModelObject* obj)
{
    vtkSmartPointer<vtkActor> actor = _objectToActor.value(obj->id());
    if (!actor) return;

    const auto& p = obj->placement();
    actor->SetPosition(p.posX, p.posY, p.posZ);
    actor->SetOrientation(p.rotX, p.rotY, p.rotZ);
    actor->SetScale(p.scaleX, p.scaleY, p.scaleZ);
}

void SceneDocument::removeActor(slicing::ModelObject* obj)
{
    auto it = _objectToActor.find(obj->id());
    if (it != _objectToActor.end())
    {
        vtkSmartPointer<vtkActor> actor = it.value();
        if (_renderer)
            _renderer->RemoveActor(actor);
        _actorToObjectId.remove(actor.Get());
        _objectToActor.erase(it);
        // vtkSmartPointer handles cleanup automatically
    }
}

CommandStack* SceneDocument::commandStack() const
{
    return _commandStack.get();
}

} // namespace csengine
