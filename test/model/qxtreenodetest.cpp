#include "qxtreenodetest.h"

#include <QObject>
#include "qtx3model.h"
#include "qtx3node.h"
QXTreeNodeTest::QXTreeNodeTest() {
  model = new QTX3Model();

  node = new QTX3Node(qobject_cast<QObject*>(model));
}

QXTreeNodeTest::~QXTreeNodeTest() {
  node->deleteLater();
  model->deleteLater();
}

TEST_F(QXTreeNodeTest, test_xPath) {
  ASSERT_EQ(QString("/*[1]"), node->xPath());
}
