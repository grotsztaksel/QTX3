
#include "qtest_qtx3model.h"
#include "qtx3testcasemodel.h"
#include <QtTest/QAbstractItemModelTester>
#include <QtTest/QTest>

using namespace QTX3;

void QTX3ModelTest::init() {
  model = QTX3TestcaseModel::createModel(this);
  new QAbstractItemModelTester(
      model, QAbstractItemModelTester::FailureReportingMode::Fatal, this);
}

void QTX3ModelTest::cleanup() { delete model; }

void QTX3ModelTest::test_constructor_with_tixihandle() {
  QCOMPARE(model->_root->rows(), 3);
  QVERIFY(model->tx > 0);

  QModelIndex i = QModelIndex();
  model->fetchMore(i);
  i = model->index(0, 0, i);
  model->fetchMore(i);
  i = model->index(0, 0, i);
  model->fetchMore(i);
  QCOMPARE(model->_root->childAt(0)->childAt(0)->columnCount(), 3);
}

void QTX3ModelTest::test_constructor_with_string() {
  Model m(nullptr, QString("extra_root"));
  QVERIFY(m._root != nullptr);
  QVERIFY(m.tx > 0);
  QCOMPARE(0, m._root->rows());
}

void QTX3ModelTest::test_index() {
  QModelIndex index = model->index(0, 0, QModelIndex());
  QVERIFY(index.isValid());
  index = model->index(1, 0, QModelIndex());
  QVERIFY(index.isValid());
  Node *node = static_cast<Node *>(index.internalPointer());
  QVERIFY(node);

  QCOMPARE("/root/child_2[1]", node->xmlPath());

  model->fetchMore(index);
  index = model->index(1, 0, index);

  QVERIFY(index.isValid());
  node = static_cast<Node *>(index.internalPointer());
  QVERIFY(node);

  QCOMPARE("/root/child_2[1]/child_2[2]", node->xmlPath());
}

void QTX3ModelTest::test_parent() {
  QModelIndex index = model->parent(QModelIndex());
  QVERIFY(!index.isValid());

  index = model->index(1, 0, QModelIndex());
  model->fetchMore(index);
  QModelIndex childindex = model->index(1, 0, index);
  QCOMPARE(index, model->parent(childindex));
}

void QTX3ModelTest::test_rowCount() {
  model->fetchMore(QModelIndex());
  QCOMPARE(3, model->rowCount(QModelIndex()));

  QModelIndex index = model->index(2, 0, QModelIndex());
  model->fetchMore(index);
  QCOMPARE(1, model->rowCount(index));

  index = model->index(0, 0, index);
  model->fetchMore(index);
  QCOMPARE(3, model->rowCount(index));
}

void QTX3ModelTest::test_columnCount() {
  QCOMPARE(model->columnCount(QModelIndex()), 1);
  QModelIndex index = model->index(2, 0, QModelIndex());
  QCOMPARE(model->columnCount(index), 1);
}

void QTX3ModelTest::test_data_f() {
  QModelIndex index = model->index(2, 0, QModelIndex());
  QCOMPARE(model->data(index), "child_2");
}

void QTX3ModelTest::test_setData() {
  QModelIndex index = model->index(2, 0, QModelIndex());
  QVERIFY(!model->setData(index, "just any value", Qt::EditRole));
}

void QTX3ModelTest::test_setFlags() {
  QModelIndex index = model->index(2, 0, QModelIndex());
  QCOMPARE(model->flags(index), Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  QCOMPARE(model->flags(QModelIndex()), Qt::NoItemFlags);
}

void QTX3ModelTest::test_addElement() {
  model->fetchMore(QModelIndex());
  QModelIndex index = model->index(1, 0); // /root/child_2[1]
  model->fetchMore(index);
  index = model->index(1, 0, index); // /root/child_2[1]/child_2[2]
  model->fetchMore(index);
  QCOMPARE(model->rowCount(index), 0);
  QCOMPARE(model->nodeFromIndex(index)->elementName(), "child_2");
  int nChild;
  QCOMPARE(
      tixiGetNumberOfChilds(model->tx, "/root/child_2[1]/child_2[2]", &nChild),
      SUCCESS);
  QCOMPARE(nChild, 0);

  // Cannot insert at index < 0, because that's ridiculous!
  QVERIFY(!model->addElement(-9, "the_thing_that_should_not_be", index));
  // Content should not have changed
  QCOMPARE(model->rowCount(index), 0);
  QCOMPARE(
      tixiGetNumberOfChilds(model->tx, "/root/child_2[1]/child_2[2]", &nChild),
      SUCCESS);
  QCOMPARE(nChild, 0);

  // Cannot insert at index > 0, because there is no children yet
  QVERIFY(!model->addElement(2, "the_thing_that_should_not_be", index));
  // Content should not have changed
  QCOMPARE(model->rowCount(index), 0);
  QCOMPARE(
      tixiGetNumberOfChilds(model->tx, "/root/child_2[1]/child_2[2]", &nChild),
      SUCCESS);
  QCOMPARE(nChild, 0);

  // Cannot insert element of non-XML compliant name
  QVERIFY(!model->addElement(0, "the thing that should not be", index));
  // Content should not have changed
  QCOMPARE(model->rowCount(index), 0);
  QCOMPARE(
      tixiGetNumberOfChilds(model->tx, "/root/child_2[1]/child_2[2]", &nChild),
      SUCCESS);
  QCOMPARE(nChild, 0);

  // Should be able to insert after all; The order and row of insertion should
  // result in sequential element numbers

  QTX3::Node *node;
  node = model->addElement(0, "elem2", index);
  QVERIFY(node);
  QCOMPARE(node->row(), 0);
  QCOMPARE(node->elementName(), "elem2");
  node = model->addElement(0, "elem0", index);
  QVERIFY(node);
  QCOMPARE(node->row(), 0);
  QCOMPARE(node->elementName(), "elem0");
  node = model->addElement(1, "elem1", index);
  QVERIFY(node);
  QCOMPARE(node->row(), 1);
  QCOMPARE(node->elementName(), "elem1");

  node = model->addElement(3, "last", index);
  QVERIFY(node);
  QCOMPARE(node->row(), 3);
  QCOMPARE(node->elementName(), "last");

  QCOMPARE(model->rowCount(index), 4);
  QCOMPARE(
      tixiGetNumberOfChilds(model->tx, "/root/child_2[1]/child_2[2]", &nChild),
      SUCCESS);
  QCOMPARE(nChild, 4);

  char *childName;
  QCOMPARE(tixiGetChildNodeName(model->tx, "/root/child_2[1]/child_2[2]", 1,
                                &childName),
           SUCCESS);
  QCOMPARE(childName, "elem0");
  QCOMPARE(tixiGetChildNodeName(model->tx, "/root/child_2[1]/child_2[2]", 2,
                                &childName),
           SUCCESS);
  QCOMPARE(childName, "elem1");
  QCOMPARE(tixiGetChildNodeName(model->tx, "/root/child_2[1]/child_2[2]", 3,
                                &childName),
           SUCCESS);
  QCOMPARE(childName, "elem2");
  QCOMPARE(tixiGetChildNodeName(model->tx, "/root/child_2[1]/child_2[2]", 4,
                                &childName),
           SUCCESS);
  QCOMPARE(childName, "last");
}

void QTX3ModelTest::test_addElement2() {
  TixiDocumentHandle h = -1;
  // This test was inspired by problems in a project using this lib
  QCOMPARE(SUCCESS, tixiCreateDocument("shooting_db", &h));
  QCOMPARE(SUCCESS, tixiCreateElement(h, "/shooting_db", "settings"));
  QCOMPARE(SUCCESS, tixiCreateElement(h, "/shooting_db", "data"));
  QCOMPARE(SUCCESS, tixiCreateElement(h, "/shooting_db/data", "dir"));
  QCOMPARE(SUCCESS, tixiCreateElement(h, "/shooting_db/data/dir", "session"));
  QCOMPARE(SUCCESS, tixiCreateElement(h, "/shooting_db/data/dir", "session"));
  QCOMPARE(SUCCESS, tixiCreateElement(h, "/shooting_db/data/dir", "session"));
  QCOMPARE(SUCCESS, tixiCreateElement(h, "/shooting_db/data/dir", "event"));

  QCOMPARE(SUCCESS, tixiAddTextAttribute(h, "/shooting_db/data/dir", "path",
                                         "../some/path"));
  QCOMPARE(SUCCESS, tixiAddTextAttribute(h, "/shooting_db/data/dir/session[1]",
                                         "timestamp", "2018-04-22T11:06:49"));
  QCOMPARE(SUCCESS, tixiAddTextAttribute(h, "/shooting_db/data/dir/session[2]",
                                         "timestamp", "2018-05-31T11:57:13"));
  QCOMPARE(SUCCESS, tixiAddTextAttribute(h, "/shooting_db/data/dir/session[3]",
                                         "timestamp", "2018-06-12T17:36:07"));
  QCOMPARE(SUCCESS, tixiAddTextAttribute(h, "/shooting_db/data/dir/event",
                                         "timestamp", "2018-12-24T18:47:16"));

  char *name;

  QTX3::Model model(nullptr, h);

  QTX3::Node *dir = model.nodeFromPath("/shooting_db/data/dir");
  QModelIndex dirIndex = dir->index();
  QVERIFY(dirIndex.isValid());
  QCOMPARE(dirIndex.row(), 0);
  model.fetchMore(dirIndex);
  QCOMPARE(4, dir->rows());
  QTX3::Node *newNode = model.addElement(4, "session", dirIndex);
  QCOMPARE(newNode->row(), 4);
  QCOMPARE(newNode->elementName(), "session");
  QCOMPARE(SUCCESS, tixiCheckElement(h, "/shooting_db/data/dir/session[1]"));
  QCOMPARE(SUCCESS, tixiGetChildNodeName(h, "/shooting_db/data/dir", 5, &name));
  QCOMPARE(name, "session");

  QCOMPARE(dir->childAt(0)->elementName(), "session");
  QCOMPARE(dir->childAt(1)->elementName(), "session");
  QCOMPARE(dir->childAt(2)->elementName(), "session");
  QCOMPARE(dir->childAt(3)->elementName(), "event");
  QCOMPARE(dir->childAt(4)->elementName(), "session");
}

void QTX3ModelTest::test_removeElement() {
  model->fetchMore(QModelIndex());
  QModelIndex index = model->index(2, 0); // /root/child_2[2]
  model->fetchMore(index);
  index = model->index(0, 0, index); // /root/child_2[2]/node_3
  model->fetchMore(index);
  QCOMPARE(model->rowCount(index), 3);
  QCOMPARE(model->nodeFromIndex(index)->elementName(), "node_3");

  QVERIFY(model->removeRows(0, 2, index));

  QCOMPARE(model->rowCount(index), 1);
  char *childName;
  QCOMPARE(
      tixiGetChildNodeName(model->tx, "/root/child_2[2]/node_3", 1, &childName),
      SUCCESS);
  QCOMPARE(childName, "node_5");
}

void QTX3ModelTest::test_createNode_baseclass() {
  auto i = QModelIndex();
  model->fetchMore(i);
  i = model->index(1, 0, i);
  model->fetchMore(i);
  i = model->index(1, 0, i);
  model->fetchMore(i);

  Node *parentNode = model->_root->childAt(1)->childAt(1);
  Node *newNode = model->Model::createNode(parentNode, QString("justName"));
  QCOMPARE(newNode->parent(), parentNode);
  QCOMPARE(newNode->model(), model);
}

void QTX3ModelTest::test_createNode_testclass() {
  Node *parentNode = model->_root->childAt(0);
  QCOMPARE("child_1", parentNode->elementName());
  Node *newNode = model->Model::createNode(parentNode, QString("justName"));
  QCOMPARE(newNode->parent(), parentNode);
  QCOMPARE(newNode->model(), model);
  QCOMPARE(newNode->columnCount(), 1);

  newNode = model->createNode(parentNode, QString("justName"));

  QCOMPARE(newNode->parent(), parentNode);
  QCOMPARE(newNode->model(), model);
  QCOMPARE(newNode->columnCount(), 3);
}

void QTX3ModelTest::test_nodeFromIndex() {
  model->fetchMore(QModelIndex());
  QModelIndex index = model->index(1, 0);
  QCOMPARE(model->nodeFromIndex(index), model->_root->childAt(1));
  model->fetchMore(index);
  index = model->index(1, 0, index);
  QCOMPARE(model->nodeFromIndex(index), model->_root->childAt(1)->childAt(1));
}

void QTX3ModelTest::test_nodeFromPath() {

  QCOMPARE(model->_root->childAt(1)->childAt(0)->childAt(0),
           model->nodeFromPath("/root/child_2[1]/child_2[1]/node_3[1]"));
  QCOMPARE(model->_root->childAt(1)->childAt(1),
           model->nodeFromPath("/root/child_2[1]/*[2]"));
  QCOMPARE(model->_root->childAt(1)->childAt(1),
           model->nodeFromPath("/*[1]/*[2]/*[2]"));
  QCOMPARE(model->_root->childAt(1)->childAt(0)->childAt(0),
           model->nodeFromPath("//*[@attr=\"9\"]/node_3[1]"));
  try {
    auto node = model->nodeFromPath("/root/child_2[1]/child_2[1]/noodle_3[1]");
    // Should have thrown error. If we're still here, the test should fail
    QVERIFY(false);
  } catch (const std::runtime_error &e) {
    QCOMPARE(e.what(),
             "nodeFromPath: failed to convert path "
             "/root/child_2[1]/child_2[1]/noodle_3[1] to indexed XPath (TiXi "
             "error code: 1)");
  }
}

void QTX3ModelTest::test_indexFromNode() {
  model->fetchMore(QModelIndex());
  QModelIndex index = model->index(1, 0);
  model->fetchMore(index);
  index = model->index(0, 0, index);
  model->fetchMore(index);
  index = model->index(0, 0, index);
  model->fetchMore(index);
  auto node = model->_root->childAt(1)->childAt(0)->childAt(0);
  QCOMPARE(model->indexFromNode(node), index);
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

void QTX3ModelTest::test_reset() {

  model->fetchMore(QModelIndex());
  model->fetchMore(model->index(0, 0));
  model->fetchMore(model->index(1, 0));

  // First check, if the structure is as expected
  QCOMPARE("/root/child_1", model->_root->childAt(0)->xmlPath());
  QCOMPARE("/root/child_1/child",
           model->_root->childAt(0)->childAt(0)->xmlPath());
  QCOMPARE("/root/child_2[1]", model->_root->childAt(1)->xmlPath());
  QCOMPARE("/root/child_2[1]/child_2[1]",
           model->_root->childAt(1)->childAt(0)->xmlPath());
  QCOMPARE("/root/child_2[1]/node_3",
           model->_root->childAt(1)->childAt(2)->xmlPath());

  // reset without valid handle
  model->reset();
  model->fetchMore(QModelIndex());
  model->fetchMore(model->index(0, 0));
  model->fetchMore(model->index(1, 0));

  // nothing should have changed
  QCOMPARE("/root/child_1", model->_root->childAt(0)->xmlPath());
  QCOMPARE("/root/child_1/child",
           model->_root->childAt(0)->childAt(0)->xmlPath());
  QCOMPARE("/root/child_2[1]", model->_root->childAt(1)->xmlPath());
  QCOMPARE("/root/child_2[1]/child_2[1]",
           model->_root->childAt(1)->childAt(0)->xmlPath());
  QCOMPARE("/root/child_2[1]/node_3",
           model->_root->childAt(1)->childAt(2)->xmlPath());

  TixiDocumentHandle th = -1;
  model->reset(th);
  model->fetchMore(QModelIndex());
  model->fetchMore(model->index(0, 0));
  model->fetchMore(model->index(1, 0));

  // nothing should have changed
  QCOMPARE("/root/child_1", model->_root->childAt(0)->xmlPath());
  QCOMPARE("/root/child_1/child",
           model->_root->childAt(0)->childAt(0)->xmlPath());
  QCOMPARE("/root/child_2[1]", model->_root->childAt(1)->xmlPath());
  QCOMPARE("/root/child_2[1]/child_2[1]",
           model->_root->childAt(1)->childAt(0)->xmlPath());
  QCOMPARE("/root/child_2[1]/node_3",
           model->_root->childAt(1)->childAt(2)->xmlPath());

  tixiCreateDocument("brave_new_root", &th);
  model->reset(th);
  model->fetchMore(QModelIndex());
  model->fetchMore(model->index(0, 0));
  model->fetchMore(model->index(1, 0));

  QCOMPARE("/brave_new_root", model->_root->xmlPath());
  QCOMPARE(model->_root->rows(), 0);

  tixiCreateElement(th, "/brave_new_root", "new_better_child");
  QCOMPARE(model->_root->rows(), 0);

  // Reset with the same th, but it has been changed outside the model
  model->reset(th);
  model->fetchMore(QModelIndex());
  model->fetchMore(model->index(0, 0));
  model->fetchMore(model->index(1, 0));

  QCOMPARE(model->_root->rows(), 1);
}

void QTX3ModelTest::test_tixi() {
  TixiDocumentHandle th = model->tixi();

  ReturnCode ret = tixiCheckElement(th, "/root/child_1/child");
  QCOMPARE(ret, SUCCESS);
}
