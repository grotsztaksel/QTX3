#ifndef QXTREENODETEST_H
#define QXTREENODETEST_H

#include "gtest/gtest.h"

class QTX3Node;
class QTX3Model;
class QObject;

class QXTreeNodeTest : public ::testing::Test {
 protected:
  QXTreeNodeTest();
  ~QXTreeNodeTest() override;

 protected:
  QTX3Node* node;
  QObject* model;
};

#endif  // QXTREENODETEST_H
