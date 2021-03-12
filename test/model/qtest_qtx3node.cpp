#include "qtest_qtx3node.h"
#include <QtTest/QTest>
#include "qtx3testcasemodel.h"

void QTX3NodeTest::init() {
  model = QTX3TestcaseModel::createModel(this);

  rootNode = new QTX3Node(model);

  rootNode->createChildren();
  node = rootNode->childAt(1)->childAt(0)->childAt(0);
}

void QTX3NodeTest::cleanup() {
  delete model;
}

void QTX3NodeTest::test_rows() {
  QCOMPARE(rootNode->rows(), 3);
  QCOMPARE(node->rows(), 4);
  auto nextNode = node->childAt(2);
  QCOMPARE(nextNode->rows(), 0);
}

void QTX3NodeTest::test_columnCount() {
  QCOMPARE(rootNode->columnCount(), 1);
  QCOMPARE(node->columnCount(), 1);
  auto nextNode = node->childAt(2);
  QCOMPARE(nextNode->columnCount(), 1);

  nextNode = rootNode->childAt(0)->childAt(0);
  QCOMPARE(nextNode->columnCount(), 3);
}

void QTX3NodeTest::test_row() {
  QCOMPARE(rootNode->row(), 0);
  QCOMPARE(node->row(), 0);
  auto nextNode = node->childAt(2);
  QCOMPARE(nextNode->row(), 2);
}

void QTX3NodeTest::test_xPath() {
  QCOMPARE(rootNode->xPath(), "/*[1]");
  QCOMPARE(node->xPath(), "/*[1]/*[2]/*[1]/*[1]");
}

void QTX3NodeTest::test_xmlPath() {
  QCOMPARE(rootNode->xmlPath(), "/root");
  QCOMPARE(node->xmlPath(), "/root/child_2[1]/child_2[1]/node_3[1]");
}

void QTX3NodeTest::test_elementName() {
  QCOMPARE(rootNode->elementName(), "root");
  QCOMPARE(node->elementName(), "node_3");
  auto nextNode = node->childAt(2);
  QCOMPARE(nextNode->elementName(), "node_5");
}

void QTX3NodeTest::test_model() {
  QCOMPARE(rootNode->model(), model);
  QCOMPARE(node->model(), model);
}

void QTX3NodeTest::test_parent() {
  QCOMPARE(rootNode->parent(), nullptr);
  QCOMPARE(node->parent(), rootNode->childAt(1)->childAt(0));
}
