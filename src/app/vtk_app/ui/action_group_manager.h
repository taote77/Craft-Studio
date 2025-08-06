#ifndef ACTION_GROUP_MANAGER_H
#define ACTION_GROUP_MANAGER_H

#include <QList>
#include <QMap>
#include <QObject>

class QActionGroup;
class QAction;
class ApplicationWindow;

namespace GUI
{

class ActionGroupManager : public QObject
{
  Q_OBJECT

public:
  explicit ActionGroupManager(ApplicationWindow* parent);

  QActionGroup* _file{ nullptr };
  QActionGroup* _modeling{ nullptr };
  QActionGroup* _widgets{ nullptr };
  QActionGroup* _pen{ nullptr };
  QActionGroup* _other{ nullptr };

  QList<QActionGroup*> toolGroups();

  QMap<QString, QActionGroup*> allGroups();

  static void sortGroupsByName(QList<QActionGroup*>& list);

public slots:
  // void toggleExclusiveSnapMode(bool state);
  // void toggleTools(bool state);

private:
  QList<bool> _snap_memory;
};

} // namespace GUI

#endif // ACTION_GROUP_MANAGER_H