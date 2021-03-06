#include "qxtreenodetest.h"

#include <QObject>
#include "qtx3model.h"
#include "qtx3node.h"

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
class QXTreeNodeTest : public ::testing::Test {
 public:
 protected:
  void SetUp() override;
  void TearDown() override;

 protected:
  QTX3Node* node;
  QTX3Model* model;
};

/*-------------------------------*
 *
 * ---test class methods---
 *
 *-------------------------------*/

void QXTreeNodeTest::SetUp() {
  model = new QTX3TestModel();
  node = new QTX3Node(model);
}

void QXTreeNodeTest::TearDown() {
  delete node;
  delete model;
}

TEST_F(QXTreeNodeTest, test_xPath) {
  ASSERT_EQ(QString("/*[1]"), node->xPath());
}

TEST_F(QXTreeNodeTest, test_Constructor_with_model) {
  ASSERT_EQ(model, node->model());
  ASSERT_EQ(model, node->parent());
}
