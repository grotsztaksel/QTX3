#include "qtest_qtx3attribute.h"
#include "qtx3attribute.h"
#include "qtx3model.h"
#include "qtx3node.h"

#include <QtTest/QTest>

void QTX3::QTX3AttributeTest::init() {
  QCOMPARE(tixiCreateDocument("root", &handle), SUCCESS);
  QCOMPARE(tixiCreateElement(handle, "/root", "child"), SUCCESS);

  model = new Model(this, handle);
}

void QTX3::QTX3AttributeTest::cleanup() {
  delete model;
  QCOMPARE(tixiCloseDocument(handle), SUCCESS);
}

void QTX3::QTX3AttributeTest::test_value() {
  QModelIndex index = model->index(0, 0);
  Attribute a(model->nodeFromIndex(index), "test_attr");

  // If the attribute is not on Tixi, should return QVariant()
  QCOMPARE(a.value(), QVariant());

  QCOMPARE(tixiAddTextAttribute(handle, "/root/child", "test_attr",
                                "attributeValue"),
           SUCCESS);
  // If the attribute is defined, should return its value
  QCOMPARE(a.value(), "attributeValue");
}
