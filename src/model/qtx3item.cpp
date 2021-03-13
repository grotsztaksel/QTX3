#include "qtx3item.h"
#include "qtx3model.h"
#include "qtx3node.h"

using namespace QTX3;

QTX3Item::QTX3Item(QTX3Node* parent)
    : QObject(parent),
      _parent(parent),
      _model(_parent->_model),
      _tixihandle(parent->_tixihandle) {}

int QTX3Item::row() const {
  return _parent->row();
}

const QTX3Node* QTX3Item::node() const {
  return parent();
}

QTX3Node* QTX3Item::parent() const {
  return _parent;
}
