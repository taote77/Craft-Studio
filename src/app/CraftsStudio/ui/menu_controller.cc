#include "menu_controller.h"
#include "mainwindow.h"
#include "plater_widget.h"

#include <csengine/scene/scene_document.hpp>
#include <csengine/command/command.hpp>

#include <QAction>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>

MenuController::MenuController(QObject* parent)
    : QObject(parent)
{
}

void MenuController::setup(QMenuBar* menuBar,
                            MainWindow* mainWindow,
                            PlaterWidget* plater,
                            csengine::SceneDocument* doc)
{
    menuBar->clear(); // Start fresh — remove any auto-generated menus
    createFileMenu(menuBar, mainWindow, plater);
    createEditMenu(menuBar, mainWindow, plater, doc);
    createViewMenu(menuBar, plater);
    createToolsMenu(menuBar, plater);
    createHelpMenu(menuBar, mainWindow);
}

QAction* MenuController::undoAction() const { return _undoAction; }
QAction* MenuController::redoAction() const { return _redoAction; }

void MenuController::updateUndoRedoState()
{
    // Updated via signal from CommandStack — handled in createEditMenu
}

// ============================================================================
// File Menu
// ============================================================================

void MenuController::createFileMenu(QMenuBar* menuBar, MainWindow* mw, PlaterWidget* plater)
{
    QMenu* menu = menuBar->addMenu("文件(&F)");

    QAction* act = menu->addAction("新建(&N)");
    act->setShortcut(QKeySequence::New);
    connect(act, &QAction::triggered, mw, &MainWindow::onNewProject);

    act = menu->addAction("打开(&O)");
    act->setShortcut(QKeySequence::Open);
    connect(act, &QAction::triggered, mw, &MainWindow::onImportFile);

    menu->addSeparator();

    act = menu->addAction("保存(&S)");
    act->setShortcut(QKeySequence::Save);
    connect(act, &QAction::triggered, mw, &MainWindow::onSaveProject);

    act = menu->addAction("另存为(&A)");
    act->setShortcut(QKeySequence::SaveAs);
    connect(act, &QAction::triggered, mw, &MainWindow::onSaveAsProject);

    menu->addSeparator();

    act = menu->addAction("导入模型...");
    connect(act, &QAction::triggered, plater, &PlaterWidget::addModel);

    act = menu->addAction("导出(&E)");
    act->setShortcut(QKeySequence("Ctrl+E"));
    connect(act, &QAction::triggered, mw, &MainWindow::onExportFile);

    menu->addSeparator();

    act = menu->addAction("退出(&X)");
    act->setShortcut(QKeySequence::Quit);
    connect(act, &QAction::triggered, mw, &MainWindow::exit);
}

// ============================================================================
// Edit Menu (with real Undo/Redo via SceneDocument CommandStack)
// ============================================================================

void MenuController::createEditMenu(QMenuBar* menuBar, MainWindow* mw,
                                     PlaterWidget* plater,
                                     csengine::SceneDocument* doc)
{
    QMenu* menu = menuBar->addMenu("编辑(&E)");

    _undoAction = menu->addAction("撤销(&U)");
    _undoAction->setShortcut(QKeySequence::Undo);
    _undoAction->setEnabled(false);
    connect(_undoAction, &QAction::triggered, this, [doc]() {
        if (doc && doc->commandStack()->canUndo())
            doc->commandStack()->undo();
    });

    _redoAction = menu->addAction("重做(&R)");
    _redoAction->setShortcut(QKeySequence::Redo);
    _redoAction->setEnabled(false);
    connect(_redoAction, &QAction::triggered, this, [doc]() {
        if (doc && doc->commandStack()->canRedo())
            doc->commandStack()->redo();
    });

    // Keep undo/redo enabled state in sync with CommandStack
    if (doc)
    {
        connect(doc->commandStack(), &csengine::CommandStack::stateChanged,
                this, [this, doc]() {
            if (_undoAction) _undoAction->setEnabled(doc->commandStack()->canUndo());
            if (_redoAction) _redoAction->setEnabled(doc->commandStack()->canRedo());
            if (doc->commandStack()->canUndo())
                _undoAction->setText(QString("撤销 %1").arg(doc->commandStack()->undoText()));
            else
                _undoAction->setText("撤销(&U)");
            if (doc->commandStack()->canRedo())
                _redoAction->setText(QString("重做 %1").arg(doc->commandStack()->redoText()));
            else
                _redoAction->setText("重做(&R)");
        });
    }

    menu->addSeparator();

    QAction* act = menu->addAction("添加模型(&A)");
    act->setShortcut(QKeySequence("Ctrl+A"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::addModel);

    act = menu->addAction("删除模型(&D)");
    act->setShortcut(QKeySequence::Delete);
    connect(act, &QAction::triggered, mw, &MainWindow::onRemoveModel);

    act = menu->addAction("复制模型(&C)");
    act->setShortcut(QKeySequence("Ctrl+D"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::duplicateSelectedModels);

    menu->addSeparator();

    act = menu->addAction("全选(&L)");
    act->setShortcut(QKeySequence::SelectAll);
    connect(act, &QAction::triggered, this, [doc]() {
        if (doc) doc->selectAll();
    });
}

// ============================================================================
// View Menu
// ============================================================================

void MenuController::createViewMenu(QMenuBar* menuBar, PlaterWidget* plater)
{
    QMenu* menu = menuBar->addMenu("视图(&V)");

    QAction* act = menu->addAction("重置视图(&R)");
    act->setShortcut(QKeySequence("Ctrl+R"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::resetCamera);

    act = menu->addAction("适配所有(&F)");
    act->setShortcut(QKeySequence("Ctrl+F"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::fitAll);

    menu->addSeparator();

    struct ViewItem { QString name; int type; QKeySequence shortcut; };
    ViewItem views[] = {
        { "顶视图",    0, QKeySequence("1") },
        { "底视图",    1, QKeySequence("2") },
        { "前视图",    2, QKeySequence("3") },
        { "左视图",    3, QKeySequence("4") },
        { "右视图",    4, QKeySequence("5") },
        { "透视视图",  5, QKeySequence("6") },
    };
    for (const auto& v : views)
    {
        act = menu->addAction(v.name);
        act->setShortcut(v.shortcut);
        connect(act, &QAction::triggered, this, [plater, type = v.type]() {
            plater->setViewType(type);
        });
    }
}

// ============================================================================
// Tools Menu
// ============================================================================

void MenuController::createToolsMenu(QMenuBar* menuBar, PlaterWidget* plater)
{
    QMenu* menu = menuBar->addMenu("工具(&T)");

    QAction* act = menu->addAction("生成支撑(&G)");
    act->setShortcut(QKeySequence("Ctrl+G"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::generateSupport);

    act = menu->addAction("移除支撑");
    act->setShortcut(QKeySequence("Ctrl+Shift+G"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::removeSupport);

    menu->addSeparator();

    act = menu->addAction("自动布局(&L)");
    act->setShortcut(QKeySequence("Ctrl+L"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::autoLayout);

    act = menu->addAction("居中模型");
    connect(act, &QAction::triggered, plater, &PlaterWidget::centerModel);

    act = menu->addAction("平放模型");
    connect(act, &QAction::triggered, plater, &PlaterWidget::layFlat);

    menu->addSeparator();

    act = menu->addAction("打印预览(&P)");
    act->setShortcut(QKeySequence("Ctrl+P"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::previewPrint);

    act = menu->addAction("导出GCode(&G)");
    act->setShortcut(QKeySequence("Ctrl+E"));
    connect(act, &QAction::triggered, plater, &PlaterWidget::generateGCode);
}

// ============================================================================
// Help Menu
// ============================================================================

void MenuController::createHelpMenu(QMenuBar* menuBar, MainWindow* mw)
{
    QMenu* menu = menuBar->addMenu("帮助(&H)");

    QAction* act = menu->addAction("关于(&A)");
    connect(act, &QAction::triggered, mw, &MainWindow::onAbout);

    act = menu->addAction("帮助(&H)");
    connect(act, &QAction::triggered, mw, &MainWindow::onHelp);
}
