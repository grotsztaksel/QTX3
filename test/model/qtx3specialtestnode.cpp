#include "qtx3specialtestnode.h"
#include "qtx3item.h"

using namespace QTX3;

QTX3specialTestNode::QTX3specialTestNode(Node* parent_node)
    : Node(parent_node), _item(new Item(this)) {}

int QTX3specialTestNode::columnCount() const {
  return 3;
}

const Item* QTX3specialTestNode::item() const {
  return _item;
}
