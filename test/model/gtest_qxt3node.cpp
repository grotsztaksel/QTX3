#include "gtest/gtest.h"

#include <QObject>
#include "qtx3model.h"
#include "qtx3node.h"
#include "qtx3testcasemodel.h"

#include "tixi.h"

/***************************************************
 *
 *
 *    THE TEST CLASS
 *
 *
 ***************************************************/
class QXT3NodeTest : public ::testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

  TixiDocumentHandle getHandle(QTX3Node* node) const;
  const QTX3Node* getParent(QTX3Node* node) const;

 protected:
  QTX3Node* node;
  QTX3Model* model;
};

/*-------------------------------*
 *
 * ---test class methods---
 *
 *-------------------------------*/

void QXT3NodeTest::SetUp() {
  model = QTX3TestcaseModel::createModel(nullptr);
  node = new QTX3Node(model);
}

void QXT3NodeTest::TearDown() {
  delete node;
  delete model;
}

TixiDocumentHandle QXT3NodeTest::getHandle(QTX3Node* node) const {
  return node->_tixihandle;
}

const QTX3Node* QXT3NodeTest::getParent(QTX3Node* node) const {
  return node->_parent;
}

/***************************************************
 *
 *    ACTUAL TESTS
 *
 **************************************************/

TEST_F(QXT3NodeTest, test_xPath) {
  ASSERT_EQ(QString("/*[1]"), node->xPath());
}

TEST_F(QXT3NodeTest, test_Constructor_with_model) {
  ASSERT_EQ(model, node->model());

  char* name;
  TixiDocumentHandle handle = getHandle(node);
  ASSERT_EQ(SUCCESS, tixiGetChildNodeName(handle, "/", 1, &name));

  ASSERT_STREQ("root", name);
}

TEST_F(QXT3NodeTest, test_Constructor_with_node) {
  QTX3Node* node2 = new QTX3Node(node);
  ASSERT_EQ(model, node2->model());
  ASSERT_EQ(node, node2->parent());
  ASSERT_EQ(node, getParent(node2));
}

TEST_F(QXT3NodeTest, test_createChildren) {
  ASSERT_EQ(3, node->createChildren());
  ASSERT_EQ(3, node->size());

  // createChildren should not work when the items are already there
  ASSERT_EQ(3, node->childAt(2)->childAt(0)->size());
  ASSERT_EQ(0, node->childAt(2)->childAt(0)->createChildren());
  ASSERT_EQ(3, node->childAt(2)->childAt(0)->size());
}

TEST_F(QXT3NodeTest, test_pathProperties) {
  ASSERT_EQ(3, node->createChildren());

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
