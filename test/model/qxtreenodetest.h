#ifndef QXTREENODETEST_H
#define QXTREENODETEST_H

#include "gtest/gtest.h"

class QXTreeNode;
class XpathTreeModel;
class QObject;

class QXTreeNodeTest : public ::testing::Test {
 protected:
  QXTreeNodeTest();
  ~QXTreeNodeTest() override;

 protected:
  QXTreeNode* node;
  QObject* model;
};

#endif  // QXTREENODETEST_H
