#include "qtx3testcasemodel.h"
#include <regex>
#include "qtx3specialtestnode.h"

QTX3TestcaseModel::QTX3TestcaseModel(QObject* parent, TixiDocumentHandle handle)
    : QTX3Model(parent, handle) {}

QTX3TestcaseModel* QTX3TestcaseModel::createModel(QObject* parent) {
  TixiDocumentHandle handle;
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
  tixiImportFromString(xml, &handle);

  return new QTX3TestcaseModel(parent, handle);
}

QTX3Node* QTX3TestcaseModel::createNode(QTX3Node* parent, const QString& name) {
  if (parent->elementName() == "child_1") {
    return new QTX3specialTestNode(parent);
  }
  return QTX3Model::createNode(parent, name);
}
