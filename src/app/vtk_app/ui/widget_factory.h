#ifndef WIDGET_FACTORY_H
#define WIDGET_FACTORY_H

#include <QMap>
#include <QObject>
#include <qmainwindow.h>

class QMenu;
class QAction;
class QMenuBar;
class QToolBar;
class QActionGroup;

class ActionHandler;
class ApplicationWindow;
class ActionGroupManager;

namespace GUI
{

class WidgetFactory : public QObject
{
  Q_OBJECT

public:
  WidgetFactory(
    ApplicationWindow* main_win, QMap<QString, QAction*>& action_map, ActionGroupManager* agm);

  void createStandardToolbars(ActionHandler* action_handler);

  void createSliceToolbars();

  void createMenus(QMenuBar* menu_bar);

  void createStatusBar(QStatusBar* menu_bar);

private:
  ApplicationWindow* _main_window = nullptr;

  QMap<QString, QAction*>& a_map;

  ActionGroupManager* _ag_manager = nullptr;

  QList<QAction*> _file_actions;
  QList<QAction*> _model_actions;
  QList<QAction*> _help_actions;
};

} // namespace GUI

#endif // WIDGET_FACTORY_H