#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QVariant>
// #include <QVariantList>
#include <memory>

// 存储树形节点的数据结构
struct ItemData
{
  QString id;   // 唯一标识
  QString name; // 显示名称

  // 构造函数
  ItemData() = default;
  ItemData(const QString& id, const QString& name)
    : id(id)
    , name(name)
  {
  }
};

class TreeItem
{
public:
  explicit TreeItem(const ItemData& data, TreeItem* parent = nullptr);
  ~TreeItem();

  // 子节点操作
  void appendChild(TreeItem* child);
  void removeChild(int row);
  TreeItem* child(int row);
  int childCount() const;
  int row() const;

  // 父节点操作
  TreeItem* parentItem();

  // 数据访问
  ItemData data() const;
  void setData(const ItemData& data);

  // 查找节点
  TreeItem* findById(const QString& id);

private:
  ItemData m_data;               // 节点数据
  TreeItem* m_parentItem;        // 父节点
  QList<TreeItem*> m_childItems; // 子节点列表（智能指针管理内存）
};

// 树形模型类，类似QStandardItemModel
class TreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit TreeModel(QObject* parent = nullptr);
  ~TreeModel() override;

  // 重写QAbstractItemModel接口
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& child) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(
    int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // 便捷操作接口（类似QStandardItemModel）
  bool appendRow(const ItemData& data, const QModelIndex& parent = QModelIndex());

  bool removeRow(int row, const QModelIndex& parent = QModelIndex());

  QModelIndex findIndexById(const QString& id) const;

private:
  TreeItem* getItem(const QModelIndex& index) const;

private:
  std::unique_ptr<TreeItem> m_rootItem; // 根节点（不可见）
};

#endif // TREE_MODEL_H
