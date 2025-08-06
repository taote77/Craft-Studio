#include <QActionGroup>

#include "action_group_manager.h"
#include "main_window.h"

namespace Sorting
{
bool byObjectName(QActionGroup* left, QActionGroup* right)
{
  return left->objectName() < right->objectName();
}
} // namespace Sorting

namespace GUI
{

ActionGroupManager::ActionGroupManager(ApplicationWindow* parent)
  : QObject(parent)
  , _file(new QActionGroup(this))
  , _modeling(new QActionGroup(this))
  , _widgets(new QActionGroup(this))
  , _pen(new QActionGroup(this))
  , _other(new QActionGroup(this))
{
  _file->setObjectName(QObject::tr("File"));
  _modeling->setObjectName(QObject::tr("Modeling"));
  _widgets->setObjectName(QObject::tr("Widgets"));
  _pen->setObjectName(QObject::tr("Pen"));
  _other->setObjectName(QObject::tr("Other"));

  foreach (auto const& ag, toolGroups())
  {
    connect(ag, &QActionGroup::triggered, parent, &ApplicationWindow::relayAction);
  }
}

QList<QActionGroup*> ActionGroupManager::toolGroups()
{
  QList<QActionGroup*> ag_list;
  ag_list << _file << _modeling << _widgets << _pen << _other;

  return ag_list;
}

QMap<QString, QActionGroup*> ActionGroupManager::allGroups()
{
  QList<QActionGroup*> ag_list = findChildren<QActionGroup*>();
  sortGroupsByName(ag_list);

  QMap<QString, QActionGroup*> ag_map;

  foreach (auto ag, ag_list)
  {
    ag_map[ag->objectName()] = ag;
  }

  return ag_map;
}

void ActionGroupManager::sortGroupsByName(QList<QActionGroup*>& list)
{
  std::sort(list.begin(), list.end(), Sorting::byObjectName);
}

} // namespace GUI