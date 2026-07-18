#ifndef CSENGINE_SCENE_DOCUMENT_HPP
#define CSENGINE_SCENE_DOCUMENT_HPP

#include <csengine/csengine_global.h>

#include <slicingcore/mesh/model_object.hpp>
#include <slicingcore/mesh/triangle_mesh.hpp>

#include <QObject>
#include <QList>
#include <QHash>
#include <memory>

#include <vtkSmartPointer.h>

class vtkActor;
class vtkRenderer;

// Forward declare VTK types (full include only in .cpp to keep header lightweight)
class vtkActor;
class vtkPolyData;
class vtkRenderer;

namespace csengine {

// ============================================================================
// SceneDocument — single source of truth for the 3D scene
//
// Replaces the current dual-SceneManager (V1 + V2) pattern.
//
// Responsibilities:
//   - Owns all ModelObjects currently on the build plate
//   - Manages VTK actor lifecycle (create, update, destroy)
//   - Manages selection state (always multi-select capable)
//   - All mutations go through Command objects (undo/redo support)
//   - Emits signals for UI synchronization
//
// NOT a singleton — instantiated by the application and passed to UI components.
// This makes unit testing possible without global state.
// ============================================================================

class Command;

class CSENGINE_API SceneDocument : public QObject
{
    Q_OBJECT

public:
    explicit SceneDocument(QObject* parent = nullptr);
    ~SceneDocument() override;

    // -- Model Management ----------------------------------------------------

    /// Add a model to the scene. Takes ownership of the mesh data.
    /// Returns the new ModelObject (owned by the document).
    slicing::ModelObject* addModel(slicing::TriangleMesh mesh,
                                    const QString& name = QString());

    /// Add a model with pre-built VTK polydata (avoids redundant meshToVTK conversion).
    /// The polydata is used directly for rendering; mesh is stored for slicing.
    slicing::ModelObject* addModelWithVTK(slicing::TriangleMesh mesh,
                                           vtkPolyData* polyData,
                                           const QString& name = QString());

    /// Remove models from the scene. Objects are deleted.
    void removeModels(const QList<slicing::ModelObject*>& objects);

    /// Remove all models
    void clearAll();

    /// Get all objects (const and mutable access)
    const QList<slicing::ModelObject*>& objects() const;

    /// Find an object by ID. Returns nullptr if not found.
    slicing::ModelObject* findObject(uint64_t id) const;

    /// Find the object associated with a VTK actor. Returns nullptr if not found.
    slicing::ModelObject* findObjectByActor(vtkActor* actor) const;

    // -- Selection -----------------------------------------------------------

    /// Current selection (always multi-select capable)
    QList<slicing::ModelObject*> selection() const;

    /// Select a single object (clears previous selection unless additive)
    void setSelected(slicing::ModelObject* obj, bool additive = false);

    /// Add to or remove from selection
    void addToSelection(slicing::ModelObject* obj);
    void removeFromSelection(slicing::ModelObject* obj);

    /// Clear all selection
    void clearSelection();

    /// Select all objects
    void selectAll();

    /// Whether any object is selected
    bool hasSelection() const;

    /// Check for 2D collisions between models on the build plate.
    /// Returns a list of colliding object pairs (names).
    struct Collision { slicing::ModelObject* a; slicing::ModelObject* b; };
    QList<Collision> checkCollisions() const;

    // -- Transform (convenience methods that create Commands internally) -----

    /// Translate selected objects by a delta (mm)
    void translate(const QList<slicing::ModelObject*>& objects,
                   double dx, double dy, double dz);

    /// Rotate selected objects (Euler angles in degrees)
    void rotate(const QList<slicing::ModelObject*>& objects,
                double rx, double ry, double rz);

    /// Scale selected objects
    void scale(const QList<slicing::ModelObject*>& objects,
               double sx, double sy, double sz);

    // -- VTK Synchronization -------------------------------------------------

    /// Set the VTK renderer (required for actor management)
    void setRenderer(vtkRenderer* renderer);

    /// Sync all VTK actors to match the current data model state
    /// Call after batch mutations or when renderer changes
    void syncToRenderer();

    /// Update actor properties for the current selection (color, etc.)
    void syncSelectionAppearance();

    /// Get the VTK actor for an object (creates one if not yet built)
    vtkActor* actorForObject(slicing::ModelObject* obj);

    // -- Undo/Redo -----------------------------------------------------------

    class CommandStack* commandStack() const;

signals:
    /// Emitted when objects are added to the scene
    void objectsAdded(const QList<slicing::ModelObject*>& objects);

    /// Emitted before objects are removed
    void objectsAboutToBeRemoved(const QList<slicing::ModelObject*>& objects);

    /// Emitted after objects are removed (objects already deleted!)
    void objectsRemoved();

    /// Emitted when selection changes
    void selectionChanged(const QList<slicing::ModelObject*>& selection);

    /// Emitted when an object's data is modified (transform, name, etc.)
    void objectModified(slicing::ModelObject* obj);

    /// Emitted when the command stack state changes (undo/redo availability)
    void commandStateChanged();

private:
    friend class Command;
    friend class AddModelCommand;
    friend class RemoveModelCommand;
    friend class TranslateCommand;
    friend class RotateCommand;

    // Internal methods used by Command subclasses
    void internalAddObject(slicing::ModelObject* obj);
    void internalRemoveObject(slicing::ModelObject* obj);
    void updateActorTransform(slicing::ModelObject* obj);

    void createActor(slicing::ModelObject* obj);
    void createActorFromVTK(slicing::ModelObject* obj, vtkPolyData* polyData);
    void removeActor(slicing::ModelObject* obj);

    QList<slicing::ModelObject*> _objects;
    QList<slicing::ModelObject*> _selection;

    // VTK rendering state
    vtkRenderer* _renderer = nullptr;
    QHash<uint64_t, vtkSmartPointer<vtkActor>> _objectToActor;
    QHash<vtkActor*, uint64_t> _actorToObjectId;

    // Undo/redo
    std::unique_ptr<CommandStack> _commandStack;
};

} // namespace csengine

#endif // CSENGINE_SCENE_DOCUMENT_HPP
