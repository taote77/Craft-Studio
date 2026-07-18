#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H

#include <QObject>

class QAction;
class QMenu;
class QMenuBar;
class MainWindow;
class PlaterWidget;

namespace csengine { class SceneDocument; }

// ============================================================================
// MenuController — single source of truth for ALL menu creation
//
// Replaces:
//   - MainWindow::createFileMenu/EditMenu/ViewMenu/ToolsMenu/HelpMenu (5 methods)
//   - MenuManager::init() (duplicate menu system)
//
// All menu actions are created here. Undo/Redo are wired to SceneDocument's
// CommandStack for real undo/redo support.
// ============================================================================

class MenuController : public QObject
{
    Q_OBJECT

public:
    explicit MenuController(QObject* parent = nullptr);

    /// Build all menus on the given menu bar. Call once during setup.
    void setup(QMenuBar* menuBar,
               MainWindow* mainWindow,
               PlaterWidget* plater,
               csengine::SceneDocument* doc);

    // -- Accessors for external use (e.g. status tips) --
    QAction* undoAction() const;
    QAction* redoAction() const;

private slots:
    void updateUndoRedoState();

private:
    void createFileMenu(QMenuBar* menuBar, MainWindow* mw, PlaterWidget* plater);
    void createEditMenu(QMenuBar* menuBar, MainWindow* mw, PlaterWidget* plater, csengine::SceneDocument* doc);
    void createViewMenu(QMenuBar* menuBar, PlaterWidget* plater);
    void createToolsMenu(QMenuBar* menuBar, PlaterWidget* plater);
    void createHelpMenu(QMenuBar* menuBar, MainWindow* mw);

    QAction* _undoAction = nullptr;
    QAction* _redoAction = nullptr;
};

#endif // MENU_CONTROLLER_H
