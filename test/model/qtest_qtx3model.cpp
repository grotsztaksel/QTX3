
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
}

void QTX3ModelTest::test_constructor_with_string() {
  QTX3Model m(nullptr, QString("extra_root"));
  QVERIFY(m._root != nullptr);
  QVERIFY(m._tixihandle > 0);
  QCOMPARE(0, m._root->rows());
}

void QTX3ModelTest::test_index() {
  QModelIndex index = model->index(1, 0, QModelIndex());
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

QTEST_MAIN(QTX3ModelTest)
