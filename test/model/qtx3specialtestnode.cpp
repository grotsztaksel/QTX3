#include "qtx3specialtestnode.h"

using namespace QTX3;

QTX3specialTestNode::QTX3specialTestNode(Node* parent_node)
    : Node(parent_node) {}

int QTX3specialTestNode::columnCount() const {
  return 3;
}
