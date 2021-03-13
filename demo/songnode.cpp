#include "songnode.h"
#include "QAbstractItemModel"

SongNode::SongNode(QTX3::QTX3Node* parent_node) : QTX3::QTX3Node(parent_node) {}

int SongNode::columnCount() const {
  return 2;
}

QVariant SongNode::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole && index.column() == 1) {
    return "Lalala";
  }
  return QTX3::QTX3Node::data(index, role);
}
