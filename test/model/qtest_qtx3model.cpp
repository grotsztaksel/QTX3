
#include "qtest_qtx3model.h"
#include <QtTest/QAbstractItemModelTester>
#include <QtTest/QTest>
#include "qtx3testcasemodel.h"

void QTX3ModelTest::test_anything() {
  QCOMPARE(1, 1);
}

// void QTX3ModelTest::TestModel() {
//  //  QTX3TestcaseModel* model = new QTX3TestcaseModel(this);
//  //  new QAbstractItemModelTester(
//  //      model, QAbstractItemModelTester::FailureReportingMode::Fatal, this);
//}

QTEST_MAIN(QTX3ModelTest)
