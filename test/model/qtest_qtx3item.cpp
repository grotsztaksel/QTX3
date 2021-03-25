#include "qtest_qtx3item.h"
#include "qtx3item.h"
#include "qtx3specialtestnode.h"
#include "qtx3testcasemodel.h"

#include <QtTest/QTest>
using namespace QTX3;
void QTX3ItemTest::init() {
  model = QTX3TestcaseModel::createModel(this);
  node = qobject_cast<QTX3specialTestNode *>(
      model->nodeFromPath("/*[1]/*[1]/*[1]"));

  QVERIFY(node);
}

void QTX3ItemTest::cleanup() { delete model; }

void QTX3ItemTest::test_row() {
  auto item = node->item();
  QCOMPARE(item->row(), 0);
}

void QTX3ItemTest::test_parent() {
  auto item = node->item();
  QCOMPARE(item->parent(), node);
}

void QTX3ItemTest::test_node() {
  auto item = node->item();
  QCOMPARE(item->node(), node);
  QCOMPARE(item->node(), item->parent());
}
