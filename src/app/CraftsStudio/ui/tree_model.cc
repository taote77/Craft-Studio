#include "tree_model.h"

// #include <qobjectdefs.h>

TreeItem::TreeItem(const ItemData& data, TreeItem* parent)
  : m_data(data)
  , m_parentItem(parent)
{
}

TreeItem::~TreeItem()
{
  // 子节点由unique_ptr管理，自动释放
}

void TreeItem::appendChild(TreeItem* child)
{
  if (child)
  {
    m_childItems.push_back(child);
  }
}

void TreeItem::removeChild(int row)
{
  if (row >= 0 && row < m_childItems.size())
  {
    m_childItems.removeAt(row);
  }
}

TreeItem* TreeItem::child(int row)
{
  if (row < 0 || row >= m_childItems.size())
  {
    return nullptr;
  }
  return m_childItems[row];
}

int TreeItem::childCount() const
{
  return m_childItems.size();
}

int TreeItem::row() const
{
  if (m_parentItem)
  {
    // 查找当前节点在父节点中的位置
    auto& siblings = m_parentItem->m_childItems;
    for (int i = 0; i < siblings.size(); ++i)
    {
      if (siblings[i] == this)
      {
        return i;
      }
    }
  }
  return 0;
}

TreeItem* TreeItem::parentItem()
{
  return m_parentItem;
}

ItemData TreeItem::data() const
{
  return m_data;
}

void TreeItem::setData(const ItemData& data)
{
  m_data = data;
}

TreeItem* TreeItem::findById(const QString& id)
{
  // 先检查当前节点
  if (m_data.id == id)
  {
    return this;
  }
  // 递归检查子节点
  for (const auto& child : m_childItems)
  {
    TreeItem* found = child->findById(id);
    if (found)
    {
      return found;
    }
  }
  return nullptr;
}

TreeModel::TreeModel(QObject* parent)
  : QAbstractItemModel(parent)
{
  // 创建根节点（数据为空，不可见）
  m_rootItem = std::make_unique<TreeItem>(ItemData("", ""));
}

TreeModel::~TreeModel() = default;

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
  {
    return QModelIndex();
  }

  auto parentItem = getItem(parent);
  auto childItem = parentItem->child(row);
  if (childItem)
  {
    return createIndex(row, column, childItem);
  }
  return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex& child) const
{
  if (!child.isValid())
  {
    return QModelIndex();
  }

  auto childItem = getItem(child);
  auto parentItem = childItem->parentItem();

  if (parentItem == m_rootItem.get() || !parentItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex& parent) const
{
  auto parentItem = getItem(parent);
  return parentItem ? parentItem->childCount() : 0;
}

int TreeModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1; // 只需要一列显示名称
}

QVariant TreeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }

  TreeItem* item = getItem(index);
  if (!item)
  {
    return QVariant();
  }

  // 根据角色返回不同数据
  switch (role)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
      return item->data().name; // 显示和编辑时返回名称
    case Qt::UserRole:
      return item->data().id; // UserRole返回ID
    default:
      return QVariant();
  }
}

bool TreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
  {
    return false;
  }

  TreeItem* item = getItem(index);
  if (!item)
  {
    return false;
  }

  // 更新数据
  ItemData data = item->data();
  bool data_changed = false;

  switch (role)
  {
    case Qt::EditRole:
    {
      QString newName = value.toString();
      if (data.name != newName)
      {
        data.name = newName;
        data_changed = true;
      }
      break;
    }
    case Qt::UserRole:
    {
      QString newId = value.toString();
      if (data.id != newId)
      {
        data.id = newId;
        data_changed = true;
      }
      break;
    }
    default:
      return false;
  }

  // 通知视图数据已更改
  if (data_changed)
  {
    item->setData(data);
    Q_EMIT dataChanged(index, index, { role });
    return true;
  }

  return false;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  // 允许选中和编辑
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return "对象列表"; // 表头名称
  }
  return QVariant();
}

bool TreeModel::appendRow(const ItemData& data, const QModelIndex& parent)
{
  auto parentItem = getItem(parent);
  if (!parentItem)
  {
    return false;
  }

  int row = parentItem->childCount();
  beginInsertRows(parent, row, row);
  {
    // 添加新节点
    parentItem->appendChild(new TreeItem(data, parentItem));
  }
  endInsertRows();
  return true;
}

bool TreeModel::removeRow(int row, const QModelIndex& parent)
{
  auto parentItem = getItem(parent);
  if (!parentItem || row < 0 || row >= parentItem->childCount())
  {
    return false;
  }

  // 通知模型即将删除行
  beginRemoveRows(parent, row, row);

  // 删除节点
  parentItem->removeChild(row);

  // 通知模型删除完成
  endRemoveRows();
  return true;
}

QModelIndex TreeModel::findIndexById(const QString& id) const
{
  auto foundItem = m_rootItem->findById(id);
  if (foundItem && foundItem != m_rootItem.get())
  {
    return createIndex(foundItem->row(), 0, foundItem);
  }
  return QModelIndex();
}

TreeItem* TreeModel::getItem(const QModelIndex& index) const
{
  if (index.isValid())
  {
    auto item = static_cast<TreeItem*>(index.internalPointer());
    if (item)
    {
      return item;
    }
  }
  return m_rootItem.get();
}
