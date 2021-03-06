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
