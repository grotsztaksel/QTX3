
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

void QTX3ModelTest::test_anything() {
  QCOMPARE(1, 1);
}

QTEST_MAIN(QTX3ModelTest)
