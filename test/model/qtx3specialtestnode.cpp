#include "qtx3specialtestnode.h"

QTX3specialTestNode::QTX3specialTestNode(QTX3Node* parent_node)
    : QTX3Node(parent_node) {
  createItems();
}

void QTX3specialTestNode::createItems() {
  // Will have 3 nodes instead of 1. Base constructor already calls base
  // createItems(), so 2x is enough here
  _columnItems.append(new QTX3Item(this));
  _columnItems.append(new QTX3Item(this));
}
