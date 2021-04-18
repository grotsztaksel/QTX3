#include "qtest_qtx3node.h"
#include "qtx3testcasemodel.h"
#include <QtTest/QTest>

using namespace QTX3;

void QTX3NodeTest::init() {
  model = QTX3TestcaseModel::createModel(this);

  rootNode = new Node(model);

  rootNode->createChildren();
  node = rootNode->childAt(1)->childAt(0)->childAt(0);
}

void QTX3NodeTest::cleanup() { delete model; }

void QTX3NodeTest::test_createChildren() {
  // First, create some extra XMl elements in tixi
  for (int i = 0; i < 3; i++) {
    QCOMPARE(SUCCESS, tixiCreateElement(*node->tx,
                                        "/root/child_1/child", "extra"));
  }
  auto cnode = rootNode->childAt(0)->childAt(0);
  QCOMPARE(cnode->rows(), 0);
  cnode->createChildren();
  QCOMPARE(cnode->rows(), 3);
  for (int i = 0; i < 3; i++) {
    tixiCreateElement(*node->tx, "/root/child_1/child", "extra");
  }

  cnode->createChildren();
  QCOMPARE(cnode->rows(), 6);
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

void QTX3NodeTest::test_index() {
  QModelIndex index = model->index(1, 0);
  index = model->index(0, 0, index);
  index = model->index(0, 0, index);
  QCOMPARE(node->index(), index);
}

void QTX3NodeTest::test_data_f() {
  auto cnode = rootNode->childAt(0)->childAt(0);
  auto index = model->index(0, 0);   // /root/child_1
  index = model->index(0, 0, index); // /root/child_1/child
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
  auto index = model->index(0, 0);   // /root/child_1
  index = model->index(0, 0, index); // /root/child_1/child
  QCOMPARE(cnode->columnCount(), 3);

  QCOMPARE(node->flags(index), Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  index = index.siblingAtColumn(2);
  QVERIFY(!index.isValid());
  QCOMPARE(node->flags(index), Qt::NoItemFlags);
}

void QTX3NodeTest::test_insertChild() {
  auto node0 = node->childAt(0);
  auto node1 = node->childAt(1);
  auto node2 = node->childAt(2);
  auto node3 = node->childAt(3);
  auto newNode = new Node(node);
  node->insertChild(newNode, 2);

  QCOMPARE(node->childAt(0), node0);
  QCOMPARE(node->childAt(1), node1);
  QCOMPARE(node->childAt(2), newNode);
  QCOMPARE(node->childAt(3), node2);
  QCOMPARE(node->childAt(4), node3);
}

void QTX3NodeTest::test_removeChildren() {

  auto node0 = node->childAt(0);
  auto node1 = node->childAt(1);
  auto node2 = node->childAt(2);
  auto node3 = node->childAt(3);

  // Since the _children is not a vector of smart pointers, even though the
  // nodes may be deleted, the pointers will not. therefore need an indirect way
  // of determining the nodes' death. Therefore each of them gets a single
  // child. When deleted, the QObject deletes all its children. The number of
  // children under pointers will indicate whether the given node is dead or
  // alive
  for (int i = 0; i < node->rows(); i++) {
    auto newNode = new Node(node->childAt(i));
    node->childAt(i)->insertChild(newNode, 0);
  }

  QCOMPARE(node0->children().size(), 1);
  QCOMPARE(node1->children().size(), 1);
  QCOMPARE(node2->children().size(), 1);
  QCOMPARE(node3->children().size(), 1);

  node->removeChildren(1, 2);

  QCOMPARE(node0->children().size(), 1);
  QCOMPARE(node1->children().size(), 0);
  QCOMPARE(node2->children().size(), 0);
  QCOMPARE(node3->children().size(), 1);
}
