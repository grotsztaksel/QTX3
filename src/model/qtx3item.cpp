#include "qtx3item.h"
#include "qtx3model.h"
#include "qtx3node.h"

QTX3Item::QTX3Item(QTX3Node* parent)
    : QObject(parent),
      _parent(parent),
      _model(_parent->_model),
      _tixihandle(parent->_tixihandle) {}

int QTX3Item::column() const {
  for (int i = 0; i < _parent->_columnItems.size(); i++) {
    if (_parent->_columnItems.at(i) == this)
      return i;
  }
  return -1;
  /* Cannot use the following:
      return _parent->_columnItems.indexOf(this);
    because it is illegal:
    * error: reference to type 'QTX3Item *const' could not bind to an rvalue of
    type 'const QTX3Item *'
    * error: invalid conversion from 'const QTX3Item*' to 'QTX3Item*'
    [-fpermissive]
  */
}

int QTX3Item::row() const {
  return _parent->row();
}

const QTX3Node* QTX3Item::node() const {
  return _parent;
}

QVariant QTX3Item::data(int role) const {
  if (role == Qt::DisplayRole && column() == 0) {
    // Default implementation displays element name
    return QVariant(parent()->elementName());
  }
  return QVariant();
}

QVector<int> QTX3Item::setData(QVariant value, int role) {
  return QVector<int>();
}

Qt::ItemFlags QTX3Item::flags() const {
  auto flags = Qt::ItemIsEnabled;
  if (column() != 0)
    flags | Qt::ItemNeverHasChildren;
  return flags;
}

QTX3Node* QTX3Item::parent() const {
  return _parent;
}
