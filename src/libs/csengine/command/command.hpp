#ifndef CSENGINE_COMMAND_HPP
#define CSENGINE_COMMAND_HPP

#include <csengine/csengine_global.h>

#include <QObject>
#include <QString>
#include <QList>
#include <vector>

#include <slicingcore/mesh/triangle_mesh.hpp>
#include <slicingcore/mesh/model_object.hpp>

namespace csengine {

class SceneDocument;

// ============================================================================
// Command — base class for undoable operations
//
// Every mutation on SceneDocument goes through a Command. Commands are
// executed immediately and pushed onto the CommandStack for potential undo.
// Each command stores enough state to reverse its effect.
// ============================================================================

class CSENGINE_API Command
{
public:
    Command(const QString& name, SceneDocument* document);
    virtual ~Command();

    /// Execute the command (called once, immediately)
    virtual void execute() = 0;

    /// Undo the command (restore previous state)
    virtual void undo() = 0;

    /// Human-readable name for undo/redo menu items
    QString name() const;

    /// The document this command operates on
    SceneDocument* document() const;

private:
    QString _name;
    SceneDocument* _document;
};

// ============================================================================
// CommandStack — undo/redo stack
// ============================================================================

class CSENGINE_API CommandStack : public QObject
{
    Q_OBJECT

public:
    explicit CommandStack(QObject* parent = nullptr);
    ~CommandStack() override;

    /// Execute a command and push it onto the undo stack.
    /// Clears the redo stack (new action invalidates redo history).
    void execute(Command* cmd);

    /// Undo the most recent command
    void undo();

    /// Redo the most recently undone command
    void redo();

    /// Check availability
    bool canUndo() const;
    bool canRedo() const;
    QString undoText() const;
    QString redoText() const;

    /// Clear all history
    void clear();

    /// Maximum number of commands to remember
    void setMaxCommands(size_t max);
    size_t maxCommands() const;

signals:
    void stateChanged();

private:
    std::vector<Command*> _undoStack;
    std::vector<Command*> _redoStack;
    size_t _maxCommands = 50;

    void trimUndoStack();
};

// ============================================================================
// Concrete Commands
// ============================================================================

/// Stores old and new position for undo
class CSENGINE_API TranslateCommand : public Command
{
public:
    TranslateCommand(SceneDocument* doc,
                     const QList<slicing::ModelObject*>& objects,
                     double dx, double dy, double dz);
    void execute() override;
    void undo() override;

private:
    struct State {
        slicing::ModelObject* obj;
        double oldX, oldY, oldZ;
    };
    std::vector<State> _states;
    double _dx, _dy, _dz;
};

/// Stores old and new rotation for undo
class CSENGINE_API RotateCommand : public Command
{
public:
    RotateCommand(SceneDocument* doc,
                  const QList<slicing::ModelObject*>& objects,
                  double rx, double ry, double rz);
    void execute() override;
    void undo() override;

private:
    struct State {
        slicing::ModelObject* obj;
        double oldRX, oldRY, oldRZ;
    };
    std::vector<State> _states;
    double _rx, _ry, _rz;
};

/// Adds a model to the scene
class CSENGINE_API AddModelCommand : public Command
{
public:
    AddModelCommand(SceneDocument* doc,
                    slicing::TriangleMesh mesh,
                    const QString& name);
    void execute() override;
    void undo() override;

private:
    slicing::TriangleMesh _mesh;
    QString _name;
    slicing::ModelObject* _addedObject = nullptr;
};

/// Removes models from the scene
class CSENGINE_API RemoveModelCommand : public Command
{
public:
    RemoveModelCommand(SceneDocument* doc,
                       const QList<slicing::ModelObject*>& objects);
    void execute() override;
    void undo() override;

private:
    struct StoredObject {
        slicing::TriangleMesh mesh;
        QString name;
        slicing::Transform3D placement;
    };
    QList<slicing::ModelObject*> _objects;
    std::vector<StoredObject> _stored;
};

} // namespace csengine

#endif // CSENGINE_COMMAND_HPP
