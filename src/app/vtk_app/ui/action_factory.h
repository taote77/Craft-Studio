#ifndef ACTION_FACTORY_H
#define ACTION_FACTORY_H

#include <QMap>
#include <QObject>

class QActionGroup;
class QAction;

class ApplicationWindow;

namespace GUI
{

class ActionGroupManager;
class ActionHandler;

class ActionFactory : public QObject
{
  Q_OBJECT

public:
  ActionFactory(ApplicationWindow* parent, ActionHandler* a_handler);

  void fillActionContainer(QMap<QString, QAction*>& a_map, ActionGroupManager* agm);

  void commonActions(QMap<QString, QAction*>& a_map, ActionGroupManager* agm);

private:
  ApplicationWindow* _main_window = nullptr;
  ActionHandler* _action_handler = nullptr;
};

} // namespace GUI

#endif // ACTION_FACTORY_H