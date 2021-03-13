#include "qtest_qtx3node.h"
#include <QtTest/QTest>
#include "qtx3testcasemodel.h"

using namespace QTX3;

void QTX3NodeTest::init() {
  model = QTX3TestcaseModel::createModel(this);

  rootNode = new Node(model);

  rootNode->createChildren();
  node = rootNode->childAt(1)->childAt(0)->childAt(0);
}

void QTX3NodeTest::cleanup() {
  delete model;
}

void QTX3NodeTest::test_createChildren() {
  // First, create some extra XMl elements in tixi
  for (int i = 0; i < 3; i++) {
    tixiCreateElement(node->_tixihandle, "/root/child_1/child", "extra");
  }
  auto cnode = rootNode->childAt(0)->childAt(0);
  QCOMPARE(cnode->rows(), 0);
  QCOMPARE(cnode->createChildren(), 3);
  QCOMPARE(cnode->rows(), 3);
  QCOMPARE(cnode->createChildren(), 0);
}

void QTX3NodeTest::test_childAt() {
  QCOMPARE(rootNode->childAt(0), rootNode->_children.at(0));
  QCOMPARE(rootNode->childAt(1), rootNode->_children.at(1));
  QCOMPARE(rootNode->childAt(2), rootNode->_children.at(2));
  QCOMPARE(node->childAt(0), node->_children.at(0));
  QCOMPARE(node->childAt(1), node->_children.at(1));
  QCOMPARE(node->childAt(2), node->_children.at(2));
  QCOMPARE(node->childAt(3), node->_children.at(3));
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

void QTX3NodeTest::test_data_f() {
  auto cnode = rootNode->childAt(0)->childAt(0);
  auto index = model->index(0, 0);    // /root/child_1
  index = model->index(0, 0, index);  // /root/child_1/child
  QCOMPARE(cnode->columnCount(), 3);
  QCOMPARE(cnode->data(index), "child");
  QCOMPARE(cnode->data(index.siblingAtColumn(2)), QVariant());
}

void QTX3NodeTest::test_setData() {
  QCOMPARE(node->setData(QModelIndex(), "just any value", Qt::EditRole),
           QVector<int>());
}

void QTX3NodeTest::test_setFlags() {
  auto cnode = rootNode->childAt(0)->childAt(0);
  auto index = model->index(0, 0);    // /root/child_1
  index = model->index(0, 0, index);  // /root/child_1/child
  QCOMPARE(cnode->columnCount(), 3);

  QCOMPARE(node->flags(index), Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  index = index.siblingAtColumn(2);
  QVERIFY(!index.isValid());
  QCOMPARE(node->flags(index), Qt::NoItemFlags);
}
