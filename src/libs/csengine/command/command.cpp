#include <csengine/command/command.hpp>
#include <csengine/scene/scene_document.hpp>

#include <QDebug>
#include <algorithm>

namespace csengine {

// ============================================================================
// Command
// ============================================================================

Command::Command(const QString& name, SceneDocument* document)
    : _name(name)
    , _document(document)
{
}

Command::~Command() = default;

QString Command::name() const
{
    return _name;
}

SceneDocument* Command::document() const
{
    return _document;
}

// ============================================================================
// CommandStack
// ============================================================================

CommandStack::CommandStack(QObject* parent)
    : QObject(parent)
{
}

CommandStack::~CommandStack()
{
    clear();
}

void CommandStack::execute(Command* cmd)
{
    cmd->execute();
    _undoStack.push_back(cmd);
    _redoStack.clear(); // new action invalidates redo
    trimUndoStack();
    emit stateChanged();
}

void CommandStack::undo()
{
    if (_undoStack.empty()) return;

    Command* cmd = _undoStack.back();
    _undoStack.pop_back();
    cmd->undo();
    _redoStack.push_back(cmd);
    emit stateChanged();
}

void CommandStack::redo()
{
    if (_redoStack.empty()) return;

    Command* cmd = _redoStack.back();
    _redoStack.pop_back();
    cmd->execute();
    _undoStack.push_back(cmd);
    emit stateChanged();
}

bool CommandStack::canUndo() const
{
    return !_undoStack.empty();
}

bool CommandStack::canRedo() const
{
    return !_redoStack.empty();
}

QString CommandStack::undoText() const
{
    if (_undoStack.empty()) return QString();
    return _undoStack.back()->name();
}

QString CommandStack::redoText() const
{
    if (_redoStack.empty()) return QString();
    return _redoStack.back()->name();
}

void CommandStack::clear()
{
    for (auto* cmd : _undoStack) delete cmd;
    for (auto* cmd : _redoStack) delete cmd;
    _undoStack.clear();
    _redoStack.clear();
    emit stateChanged();
}

void CommandStack::setMaxCommands(size_t max)
{
    _maxCommands = max;
    trimUndoStack();
}

size_t CommandStack::maxCommands() const
{
    return _maxCommands;
}

void CommandStack::trimUndoStack()
{
    while (_undoStack.size() > _maxCommands)
    {
        delete _undoStack.front();
        _undoStack.erase(_undoStack.begin());
    }
}

// ============================================================================
// TranslateCommand
// ============================================================================

TranslateCommand::TranslateCommand(SceneDocument* doc,
                                     const QList<slicing::ModelObject*>& objects,
                                     double dx, double dy, double dz)
    : Command(QString("Translate %1 objects").arg(objects.size()), doc)
    , _dx(dx), _dy(dy), _dz(dz)
{
    for (auto* obj : objects)
    {
        const auto& p = obj->placement();
        _states.push_back({ obj, p.posX, p.posY, p.posZ });
    }
}

void TranslateCommand::execute()
{
    for (auto& s : _states)
    {
        const auto& p = s.obj->placement();
        s.obj->setPosition(p.posX + _dx, p.posY + _dy, p.posZ + _dz);
        document()->updateActorTransform(s.obj);
    }
}

void TranslateCommand::undo()
{
    for (auto& s : _states)
    {
        s.obj->setPosition(s.oldX, s.oldY, s.oldZ);
        document()->updateActorTransform(s.obj);
    }
}

// ============================================================================
// RotateCommand
// ============================================================================

RotateCommand::RotateCommand(SceneDocument* doc,
                               const QList<slicing::ModelObject*>& objects,
                               double rx, double ry, double rz)
    : Command(QString("Rotate %1 objects").arg(objects.size()), doc)
    , _rx(rx), _ry(ry), _rz(rz)
{
    for (auto* obj : objects)
    {
        const auto& p = obj->placement();
        _states.push_back({ obj, p.rotX, p.rotY, p.rotZ });
    }
}

void RotateCommand::execute()
{
    for (auto& s : _states)
    {
        const auto& p = s.obj->placement();
        s.obj->setRotation(p.rotX + _rx, p.rotY + _ry, p.rotZ + _rz);
        document()->updateActorTransform(s.obj);
    }
}

void RotateCommand::undo()
{
    for (auto& s : _states)
    {
        s.obj->setRotation(s.oldRX, s.oldRY, s.oldRZ);
        document()->updateActorTransform(s.obj);
    }
}

// ============================================================================
// AddModelCommand
// ============================================================================

AddModelCommand::AddModelCommand(SceneDocument* doc,
                                   slicing::TriangleMesh mesh,
                                   const QString& name)
    : Command(QString("Add %1").arg(name.isEmpty() ? "Model" : name), doc)
    , _mesh(std::move(mesh))
    , _name(name)
{
}

void AddModelCommand::execute()
{
    auto* obj = new slicing::ModelObject(_name.toStdString());
    obj->addVolume(std::move(_mesh), _name.toStdString());
    _addedObject = obj;
    document()->internalAddObject(obj);
}

void AddModelCommand::undo()
{
    if (_addedObject)
    {
        document()->internalRemoveObject(_addedObject);
        _addedObject = nullptr;
    }
}

// ============================================================================
// RemoveModelCommand
// ============================================================================

RemoveModelCommand::RemoveModelCommand(SceneDocument* doc,
                                         const QList<slicing::ModelObject*>& objects)
    : Command(QString("Remove %1 objects").arg(objects.size()), doc)
    , _objects(objects)
{
    // Store full state for undo
    for (auto* obj : objects)
    {
        StoredObject stored;
        stored.name = QString::fromStdString(obj->name());
        stored.placement = obj->placement();
        if (obj->volumeCount() > 0)
            stored.mesh = obj->volumes()[0].mesh();
        _stored.push_back(std::move(stored));
    }
}

void RemoveModelCommand::execute()
{
    for (auto* obj : _objects)
        document()->internalRemoveObject(obj);
    _objects.clear();
}

void RemoveModelCommand::undo()
{
    for (auto& stored : _stored)
    {
        slicing::TriangleMesh mesh = stored.mesh;
        auto* obj = new slicing::ModelObject(stored.name.toStdString());
        obj->addVolume(std::move(mesh), stored.name.toStdString());
        obj->setPlacement(stored.placement);
        document()->internalAddObject(obj);
    }
    _stored.clear();
}

} // namespace csengine
