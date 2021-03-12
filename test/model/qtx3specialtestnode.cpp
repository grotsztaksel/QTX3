#include "qtx3specialtestnode.h"

QTX3specialTestNode::QTX3specialTestNode(QTX3Node* parent_node)
    : QTX3Node(parent_node) {}

int QTX3specialTestNode::columnCount() const {
  return 3;
}
