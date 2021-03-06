﻿#include "gtest/gtest.h"

#include <QObject>
#include "qtx3model.h"
#include "qtx3node.h"

#include "tixi.h"

#include <regex>

/***************************************************
 *
 *
 *    Special model used to test the nodes
 *
 *
 ***************************************************/
class QTX3TestModel : public QTX3Model {
 public:
  QTX3TestModel(QObject* parent = nullptr);
};

/*-------------------------------*
 *
 * ---model class methods---
 *
 *-------------------------------*/
QTX3TestModel::QTX3TestModel(QObject* parent) : QTX3Model(parent) {
  std::string rawstring(R"(<?xml version="1.0"?>
                            <root>
                                <child_1>
                                    <child/>
                                </child_1>
                                <child_2 attr="foo" name="bar">
                                    <child_2 attr="9">
                                        <node_3>
                                            <node_4/>
                                            <!-- some comment -->
                                            <node_4 attr="good"/>
                                            <node_5>
                                                                Text O
                                            </node_5>
                                            <node_4 name="node4">
                                                                Text
                                            </node_4>
                                        </node_3>
                                        <node_3 name="none"/>
                                    </child_2>
                                    <child_2/>
                                    <node_3>
                                        <node_4/>
                                    </node_3>
                                </child_2>
                                <child_2>
                                    <node_3>
                                        <node_4/>
                                        <node_4/>
                                        <node_5/>
                                    </node_3>
                                </child_2>
                            </root>)");
  std::regex re(R"(>\s*<)");
  std::string stripped = std::regex_replace(rawstring, re, "><");
  const char* xml = strdup(stripped.c_str());
  tixiImportFromString(xml, &_tixihandle);
}

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
  model = new QTX3TestModel();
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
  ASSERT_EQ(model, node->parent());

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
