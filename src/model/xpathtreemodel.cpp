#include "xpathtreemodel.h"

XpathTreeModel::XpathTreeModel(QObject* parent) : QAbstractItemModel(parent) {}

QVariant XpathTreeModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const {
  // FIXME: Implement me!
}

bool XpathTreeModel::setHeaderData(int section,
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

QModelIndex XpathTreeModel::index(int row,
                                  int column,
                                  const QModelIndex& parent) const {
  // FIXME: Implement me!
}

QModelIndex XpathTreeModel::parent(const QModelIndex& index) const {
  // FIXME: Implement me!
}

int XpathTreeModel::rowCount(const QModelIndex& parent) const {
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

int XpathTreeModel::columnCount(const QModelIndex& parent) const {
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

QVariant XpathTreeModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();

  // FIXME: Implement me!
  return QVariant();
}

bool XpathTreeModel::setData(const QModelIndex& index,
                             const QVariant& value,
                             int role) {
  if (data(index, role) != value) {
    // FIXME: Implement me!
    emit dataChanged(index, index, QVector<int>() << role);
    return true;
  }
  return false;
}

Qt::ItemFlags XpathTreeModel::flags(const QModelIndex& index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return Qt::ItemIsEditable;  // FIXME: Implement me!
}

bool XpathTreeModel::insertRows(int row, int count, const QModelIndex& parent) {
  beginInsertRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endInsertRows();
}

bool XpathTreeModel::insertColumns(int column,
                                   int count,
                                   const QModelIndex& parent) {
  beginInsertColumns(parent, column, column + count - 1);
  // FIXME: Implement me!
  endInsertColumns();
}

bool XpathTreeModel::removeRows(int row, int count, const QModelIndex& parent) {
  beginRemoveRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endRemoveRows();
}

bool XpathTreeModel::removeColumns(int column,
                                   int count,
                                   const QModelIndex& parent) {
  beginRemoveColumns(parent, column, column + count - 1);
  // FIXME: Implement me!
  endRemoveColumns();
}
