#include "gtest/gtest.h"

#include "qtx3model.h"
#include "qtx3node.h"
#include "qtx3testcasemodel.h"
#include <QObject>

#include "tixi.h"

/***************************************************
 *
 *
 *    THE TEST CLASS
 *
 *
 ***************************************************/

namespace QTX3 {

class QTX3NodeGTest : public ::testing::Test {
protected:
  void SetUp() override;
  void TearDown() override;

  TixiDocumentHandle getHandle(Node *node) const;
  const Node *getParent(Node *node) const;

protected:
  Node *node;
  Model *model;
};

/*-------------------------------*
 *
 * ---test class methods---
 *
 *-------------------------------*/

void QTX3NodeGTest::SetUp() {
  model = QTX3TestcaseModel::createModel(nullptr);
  node = new Node(model);
}

void QTX3NodeGTest::TearDown() {
  delete node;
  delete model;
}

TixiDocumentHandle QTX3NodeGTest::getHandle(Node *node) const {
  return *node->_tixihandle;
}

const Node *QTX3NodeGTest::getParent(Node *node) const { return node->_parent; }

/***************************************************
 *
 *    ACTUAL TESTS
 *
 **************************************************/

TEST_F(QTX3NodeGTest, test_xPath) {
  ASSERT_EQ(QString("/*[1]"), node->xPath());
}

TEST_F(QTX3NodeGTest, test_Constructor_with_model) {
  ASSERT_EQ(model, node->model());

  char *name;
  TixiDocumentHandle handle = getHandle(node);
  ASSERT_EQ(SUCCESS, tixiGetChildNodeName(handle, "/", 1, &name));

  ASSERT_STREQ("root", name);
}

TEST_F(QTX3NodeGTest, test_Constructor_with_node) {
  Node *node2 = new Node(node);
  ASSERT_EQ(model, node2->model());
  ASSERT_EQ(node, node2->parent());
  ASSERT_EQ(node, getParent(node2));
}

TEST_F(QTX3NodeGTest, test_createChildren) {
  node->createChildren();
  ASSERT_EQ(3, node->rows());
  // Create some extra XMl elements in tixi
  for (int i = 0; i < 3; i++) {
    ASSERT_EQ(SUCCESS, tixiCreateElement(getHandle(node), "/root/child_1/child",
                                         "extra"));
  }

  auto cnode = node->childAt(0)->childAt(0);
  ASSERT_EQ(cnode->rows(), 0);
  cnode->createChildren();
  ASSERT_EQ(cnode->rows(), 3);
  for (int i = 0; i < 3; i++) {
    tixiCreateElement(getHandle(node), "/root/child_1/child", "extra");
  }

  cnode->createChildren();
  ASSERT_EQ(cnode->rows(), 6);
}

TEST_F(QTX3NodeGTest, test_pathProperties) {
  node->createChildren();

  ASSERT_EQ("/*[1]/*[1]", node->childAt(0)->xPath());
  ASSERT_EQ("/root/child_1", node->childAt(0)->xmlPath());
  ASSERT_EQ(0, node->childAt(0)->row());

  ASSERT_EQ("/*[1]/*[1]/*[1]", node->childAt(0)->childAt(0)->xPath());
  ASSERT_EQ("/root/child_1/child", node->childAt(0)->childAt(0)->xmlPath());
  ASSERT_EQ(0, node->childAt(0)->childAt(0)->row());

  ASSERT_EQ("/*[1]/*[2]", node->childAt(1)->xPath());
  ASSERT_EQ("/root/child_2[1]", node->childAt(1)->xmlPath());
  ASSERT_EQ(1, node->childAt(1)->row());

  ASSERT_EQ("/*[1]/*[2]/*[1]", node->childAt(1)->childAt(0)->xPath());
  ASSERT_EQ("/root/child_2[1]/child_2[1]",
            node->childAt(1)->childAt(0)->xmlPath());
  ASSERT_EQ("child_2", node->childAt(1)->childAt(0)->elementName());

  ASSERT_EQ("/*[1]/*[2]/*[2]", node->childAt(1)->childAt(1)->xPath());

  ASSERT_EQ("/*[1]/*[2]/*[3]", node->childAt(1)->childAt(2)->xPath());
  ASSERT_EQ("/root/child_2[1]/node_3", node->childAt(1)->childAt(2)->xmlPath());
  ASSERT_EQ(2, node->childAt(1)->childAt(2)->row());

  ASSERT_EQ("/*[1]/*[2]/*[3]/*[1]",
            node->childAt(1)->childAt(2)->childAt(0)->xPath());

  ASSERT_EQ("/*[1]/*[3]", node->childAt(2)->xPath());
  ASSERT_EQ("/*[1]/*[3]/*[1]", node->childAt(2)->childAt(0)->xPath());
  ASSERT_EQ("/*[1]/*[3]/*[1]/*[1]",
            node->childAt(2)->childAt(0)->childAt(0)->xPath());
  ASSERT_EQ("/*[1]/*[3]/*[1]/*[2]",
            node->childAt(2)->childAt(0)->childAt(1)->xPath());
  ASSERT_EQ("/*[1]/*[3]/*[1]/*[3]",
            node->childAt(2)->childAt(0)->childAt(2)->xPath());
}
} // namespace QTX3
