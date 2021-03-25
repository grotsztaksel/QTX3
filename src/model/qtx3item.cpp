#include "qtx3item.h"
#include "qtx3model.h"
#include "qtx3node.h"

using namespace QTX3;

Item::Item(Node *parent)
    : QObject(parent), _parent(parent), _model(_parent->_model),
      _tixihandle(parent->_tixihandle) {}

int Item::row() const { return _parent->row(); }

const Node *Item::node() const { return parent(); }

Node *Item::parent() const { return _parent; }
