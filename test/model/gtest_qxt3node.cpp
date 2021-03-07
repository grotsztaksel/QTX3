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
  QVector<QTX3Node*> getChildrenList(const QTX3Node* node) const;

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
  model = new QTX3TestcaseModel();
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

QVector<QTX3Node*> QXT3NodeTest::getChildrenList(const QTX3Node* node) const {
  return node->_children;
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
  ASSERT_EQ(3, getChildrenList(node).size());
  // createChildren should not work when the items are already there
  auto child_2_2 = getChildrenList(node).at(2);
  auto child_2_2_node_3 = getChildrenList(child_2_2).at(0);
  ASSERT_EQ(3, getChildrenList(child_2_2_node_3).size());
  ASSERT_EQ(0, child_2_2_node_3->createChildren());
  ASSERT_EQ(3, getChildrenList(child_2_2_node_3).size());
}

TEST_F(QXT3NodeTest, test_pathProperties) {
  ASSERT_EQ(3, node->createChildren());
  auto child_1_1 = getChildrenList(node).at(0);
  ASSERT_EQ("/*[1]/*[1]", child_1_1->xPath());
  ASSERT_EQ("/root/child_1", child_1_1->xmlPath());
  ASSERT_EQ(0, child_1_1->row());
  auto child_1_1_child = getChildrenList(child_1_1).at(0);
  ASSERT_EQ("/*[1]/*[1]/*[1]", child_1_1_child->xPath());
  ASSERT_EQ("/root/child_1/child", child_1_1_child->xmlPath());
  ASSERT_EQ(0, child_1_1_child->row());
  auto child_2_1 = getChildrenList(node).at(1);
  ASSERT_EQ("/*[1]/*[2]", child_2_1->xPath());
  ASSERT_EQ("/root/child_2[1]", child_2_1->xmlPath());
  ASSERT_EQ(1, child_2_1->row());
  auto child_2_1_child_2_1 = getChildrenList(child_2_1).at(0);
  ASSERT_EQ("/*[1]/*[2]/*[1]", child_2_1_child_2_1->xPath());
  ASSERT_EQ("/root/child_2[1]/child_2[1]", child_2_1_child_2_1->xmlPath());
  ASSERT_EQ("child_2", child_2_1_child_2_1->elementName());
  auto child_2_1_child_2_2 = getChildrenList(child_2_1).at(1);
  ASSERT_EQ("/*[1]/*[2]/*[2]", child_2_1_child_2_2->xPath());
  auto child_2_1_node_3 = getChildrenList(child_2_1).at(2);
  ASSERT_EQ("/*[1]/*[2]/*[3]", child_2_1_node_3->xPath());
  ASSERT_EQ("/root/child_2[1]/node_3", child_2_1_node_3->xmlPath());
  ASSERT_EQ(2, child_2_1_node_3->row());
  auto child_2_1_node_3_node_4 = getChildrenList(child_2_1_node_3).at(0);
  ASSERT_EQ("/*[1]/*[2]/*[3]/*[1]", child_2_1_node_3_node_4->xPath());

  auto child_2_2 = getChildrenList(node).at(2);
  ASSERT_EQ("/*[1]/*[3]", child_2_2->xPath());
  auto child_2_2_node_3 = getChildrenList(child_2_2).at(0);
  ASSERT_EQ("/*[1]/*[3]/*[1]", child_2_2_node_3->xPath());
  auto child_2_2_node_3_node_4_1 = getChildrenList(child_2_2_node_3).at(0);
  ASSERT_EQ("/*[1]/*[3]/*[1]/*[1]", child_2_2_node_3_node_4_1->xPath());
  auto child_2_2_node_3_node_4_2 = getChildrenList(child_2_2_node_3).at(1);
  ASSERT_EQ("/*[1]/*[3]/*[1]/*[2]", child_2_2_node_3_node_4_2->xPath());
  auto child_2_2_node_3_node_5 = getChildrenList(child_2_2_node_3).at(2);
  ASSERT_EQ("/*[1]/*[3]/*[1]/*[3]", child_2_2_node_3_node_5->xPath());
}
