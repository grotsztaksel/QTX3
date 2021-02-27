#include "qxtreenodetest.h"

#include <QObject>
#include "qxtreenode.h"
#include "xpathtreemodel.h"
QXTreeNodeTest::QXTreeNodeTest() {
  model = new XpathTreeModel();

  node = new QXTreeNode(qobject_cast<QObject*>(model));
}

QXTreeNodeTest::~QXTreeNodeTest() {
  node->deleteLater();
  model->deleteLater();
}

TEST_F(QXTreeNodeTest, test_xPath) {
  ASSERT_EQ(QString("/*[1]"), node->xPath());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
