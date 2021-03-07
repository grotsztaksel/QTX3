#include "qtx3model.h"
#include "qtx3node.h"

QTX3Model::QTX3Model(QObject* parent, TixiDocumentHandle handle)
    : QAbstractItemModel(parent),
      _tixihandle(handle),
      _root(new QTX3Node(this)) {
  assert(handle > 0);
}

QTX3Model::QTX3Model(QObject* parent, QString rootName) {
  TixiDocumentHandle handle;
  tixiCreateDocument(rootName.toStdString().c_str(), &handle);
  QTX3Model(parent, handle);
}

QVariant QTX3Model::headerData(int section,
                               Qt::Orientation orientation,
                               int role) const {
  // FIXME: Implement me!
}

bool QTX3Model::setHeaderData(int section,
                              Qt::Orientation orientation,
                              const QVariant& value,
                              int role) {
  if (value != headerData(section, orientation, role)) {
    // FIXME: Implement me!
    emit headerDataChanged(orientation, section, section);
    return true;
  }
  return false;
}

QModelIndex QTX3Model::index(int row,
                             int column,
                             const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  const QTX3Node* parentNode;
  if (!parent.isValid()) {
    parentNode = _root;
  } else {
    parentNode = nodeFromIndex(parent);
  }
  QTX3Node* childNode = parentNode->childAt(row);
  if (!childNode)
    return QModelIndex();

  QTX3Item* columnItem = childNode->itemAt(column);

  return createIndex(row, column, columnItem);
}

QModelIndex QTX3Model::parent(const QModelIndex& index) const {
  if (!index.isValid())
    return QModelIndex();
  const QTX3Node* parentNode = nodeFromIndex(index)->parent();
  return createIndex(parentNode->row(), 0, parentNode->itemAt(0));
}

int QTX3Model::rowCount(const QModelIndex& parent) const {
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

int QTX3Model::columnCount(const QModelIndex& parent) const {
  if (!parent.isValid())
    return 0;
  // FIXME: Implement me!
}

QVariant QTX3Model::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();

  return itemFromIndex(index)->data(role);
}

bool QTX3Model::setData(const QModelIndex& index,
                        const QVariant& value,
                        int role) {
  if (data(index, role) != value) {
    QVector<int> roles = itemFromIndex(index)->setData(value, role);
    if (roles.empty()) {
      return false;
    }
    emit dataChanged(index, index, roles);
    return true;
  }
  return false;
}

Qt::ItemFlags QTX3Model::flags(const QModelIndex& index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return itemFromIndex(index)->flags();
}

bool QTX3Model::insertRows(int row, int count, const QModelIndex& parent) {
  beginInsertRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endInsertRows();
}

bool QTX3Model::insertColumns(int column,
                              int count,
                              const QModelIndex& parent) {
  beginInsertColumns(parent, column, column + count - 1);
  // FIXME: Implement me!
  endInsertColumns();
}

bool QTX3Model::removeRows(int row, int count, const QModelIndex& parent) {
  beginRemoveRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endRemoveRows();
}

bool QTX3Model::removeColumns(int column,
                              int count,
                              const QModelIndex& parent) {
  beginRemoveColumns(parent, column, column + count - 1);
  // FIXME: Implement me!
  endRemoveColumns();
}

QTX3Item* QTX3Model::itemFromIndex(QModelIndex index) const {
  return static_cast<QTX3Item*>(index.internalPointer());
}

const QTX3Node* QTX3Model::nodeFromIndex(QModelIndex index) const {
  return itemFromIndex(index)->parent();
}
