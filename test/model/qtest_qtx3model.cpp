
#include "qtest_qtx3model.h"
#include <QtTest/QAbstractItemModelTester>
#include <QtTest/QTest>
#include "qtx3testcasemodel.h"

void QTX3ModelTest::init() {
  model = QTX3TestcaseModel::createModel(this);
  new QAbstractItemModelTester(
      model, QAbstractItemModelTester::FailureReportingMode::Fatal, this);
}

void QTX3ModelTest::cleanup() {
  delete model;
}

void QTX3ModelTest::test_constructor_with_tixihandle() {
  QCOMPARE(3, model->_root->rows());
  QVERIFY(model->_tixihandle > 0);

  QCOMPARE(3, model->_root->childAt(0)->childAt(0)->columns());
}

void QTX3ModelTest::test_constructor_with_string() {
  QTX3Model m(nullptr, QString("extra_root"));
  QVERIFY(m._root != nullptr);
  QVERIFY(m._tixihandle > 0);
  QCOMPARE(0, m._root->rows());
}

void QTX3ModelTest::test_index() {
  QModelIndex index = model->index(0, 0, QModelIndex());
  QVERIFY(index.isValid());
  index = model->index(1, 0, QModelIndex());
  QVERIFY(index.isValid());
  QTX3Item* item = static_cast<QTX3Item*>(index.internalPointer());
  QVERIFY(item);

  QCOMPARE("/root/child_2[1]", item->parent()->xmlPath());

  index = model->index(1, 0, index);

  QVERIFY(index.isValid());
  item = static_cast<QTX3Item*>(index.internalPointer());
  QVERIFY(item);

  QCOMPARE("/root/child_2[1]/child_2[2]", item->parent()->xmlPath());
}

void QTX3ModelTest::test_parent() {
  QModelIndex index = model->parent(QModelIndex());
  QVERIFY(!index.isValid());

  index = model->index(1, 0, QModelIndex());
  QModelIndex childindex = model->index(1, 0, index);
  QCOMPARE(index, model->parent(childindex));
}

void QTX3ModelTest::test_rowCount() {
  QCOMPARE(3, model->rowCount(QModelIndex()));
  QModelIndex index = model->index(2, 0, QModelIndex());

  QCOMPARE(1, model->rowCount(index));

  index = model->index(0, 0, index);
  QCOMPARE(3, model->rowCount(index));
}

void QTX3ModelTest::test_columnCount() {
  QCOMPARE(0, model->columnCount(QModelIndex()));
  QModelIndex index = model->index(2, 0, QModelIndex());
  QCOMPARE(1, model->columnCount(index));

  // The child_1 and child will have special constructors
  QModelIndex child_1_index = model->index(1, 0);
  QModelIndex child_index = model->index(1, 0, child_1_index);
  QCOMPARE(1, model->columnCount(child_1_index));
  QCOMPARE(0, model->columnCount(child_index));
}

void QTX3ModelTest::test_createNode_baseclass() {
  QTX3Node* parentNode = model->_root->childAt(1)->childAt(1);
  QTX3Node* newNode =
      model->QTX3Model::createNode(parentNode, QString("justName"));
  QCOMPARE(newNode->parent(), parentNode);
  QCOMPARE(newNode->model(), model);
}

void QTX3ModelTest::test_createNode_testclass() {
  QTX3Node* parentNode = model->_root->childAt(0);
  QCOMPARE("child_1", parentNode->elementName());
  QTX3Node* newNode =
      model->QTX3Model::createNode(parentNode, QString("justName"));
  QCOMPARE(newNode->parent(), parentNode);
  QCOMPARE(newNode->model(), model);
  QCOMPARE(newNode->columns(), 1);

  newNode = model->createNode(parentNode, QString("justName"));

  QCOMPARE(newNode->parent(), parentNode);
  QCOMPARE(newNode->model(), model);
  QCOMPARE(newNode->columns(), 3);
}

void QTX3ModelTest::test_nodeFromIndex() {
  QModelIndex index = model->index(1, 0);
  QCOMPARE(model->nodeFromIndex(index), model->_root->childAt(1));
  index = model->index(1, 0, index);
  QCOMPARE(model->nodeFromIndex(index), model->_root->childAt(1)->childAt(1));

  // Navigate down to "/root/child_1/child" - it should have 3 columns
  index = model->index(0, 0);
  index = model->index(0, 2, index);
  // But pointing to the non-0 column should also return the node
  QCOMPARE(model->nodeFromIndex(index), model->_root->childAt(0)->childAt(0));
}

void QTX3ModelTest::test_nodeFromPath() {
  QCOMPARE(model->nodeFromPath("/root/child_2[1]/child_2[1]/node_3[1]"),
           model->_root->childAt(1)->childAt(0)->childAt(0));
  QCOMPARE(model->nodeFromPath("/root/child_2[1]/*[2]"),
           model->_root->childAt(1)->childAt(1));
  QCOMPARE(model->nodeFromPath("/*[1]/*[2]/*[2]"),
           model->_root->childAt(1)->childAt(1));
  QCOMPARE(model->nodeFromPath("//*[@attr=\"9\"]/node_3[1]"),
           model->_root->childAt(1)->childAt(0)->childAt(0));
  try {
    auto node = model->nodeFromPath("/root/child_2[1]/child_2[1]/noodle_3[1]");
    // Should have thrown error. If we're still here, the test should fail
    QVERIFY(false);
  } catch (const std::runtime_error& e) {
    QCOMPARE(e.what(),
             "nodeFromPath: failed to convert path "
             "/root/child_2[1]/child_2[1]/noodle_3[1] to indexed XPath (TiXi "
             "error code: 1)");
  }
}

void QTX3ModelTest::test_data() {
  QModelIndex child = model->index(0, 0);
  QCOMPARE("child_1", model->data(child, Qt::DisplayRole));
  QModelIndex index = model->index(0, 0, child);
  QCOMPARE("child", model->data(index, Qt::DisplayRole));
  index = model->index(0, 1, child);
  QCOMPARE("child", model->data(index, Qt::DisplayRole));
  index = model->index(0, 2, child);
  QCOMPARE("child", model->data(index, Qt::DisplayRole));

  index = model->index(1, 0);
  QCOMPARE("child_2", model->data(index, Qt::DisplayRole));
  index = model->index(1, 0, index);
  QCOMPARE("child_2", model->data(index, Qt::DisplayRole));
}
