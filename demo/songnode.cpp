#include "songnode.h"
#include "QAbstractItemModel"
#include "qtx3attribute.h"
SongNode::SongNode(QTX3::Node* parent_node)
    : QTX3::Node(parent_node), _title(this, "title") {}

int SongNode::columnCount() const {
  return 2;
}

QVariant SongNode::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole && index.column() == 1) {
    return _title.value();
  }
  return QTX3::Node::data(index, role);
}
