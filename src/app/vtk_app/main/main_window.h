#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QAction>
#include <QMainWindow>
#include <QMdiArea>
#include <qmenubar.h>

#include "consolewidget.h"
#include "dockdialog.h"
#include "documentview.h"
#include "workspace.h"

class QAction;
class QMdiSubWindow;

namespace GUI
{
class ActionGroupManager;
class ActionHandler;
} // namespace GUI

class ApplicationWindow : public QMainWindow
{
  Q_OBJECT

public:
  ApplicationWindow();

  /**
   * @return Pointer to application window.
   */
  static ApplicationWindow& getAppWindow();

public slots:
  void relayAction(QAction* q_action);

  void slotFileNewNew();

  void slotWindowActivatedI(int);

  void slotWindowActivated(QMdiSubWindow* sub_win);

  void slotFileNew();

  void slotFileOpen();

  void slotFileClose();

  void slotFileSave();

  void slotFileSaveAs();

  void slotImageProcess();

  void slotSliceSTL();

protected:
  void InitUi();

private:
  GUI::WorkSpace* _workspace{ nullptr };

  GUI::DocumentView* _doc_view{ nullptr };

  GUI::DockDialog* _dock_dialog{ nullptr };

  GUI::ConsoleWidget* _console{ nullptr };

  QMap<QString, QAction*> _a_map;

  GUI::ActionGroupManager* _ag_manager{ nullptr };

  GUI::ActionHandler* _action_handler{ nullptr };
};

#endif // MAIN_WINDOW_H