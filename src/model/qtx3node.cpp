#include "qtx3node.h"
#include "txutils.h"

#include "qtx3model.h"
QTX3Node::QTX3Node(QTX3Model* parent_model)
    : QObject(parent_model), _model(parent_model) {
  parent_model->giveHandle(this);
}

QTX3Node::QTX3Node(QTX3Node* parent_node)
    : QObject(parent_node),
      _model(parent_node->_model),
      _parent(parent_node),
      _tixihandle(parent_node->_tixihandle) {}

int QTX3Node::createChildren() {
  std::string xpath = xPath().toStdString() + "/*";
  int nchildren = 0;
  auto res =
      tixiXPathEvaluateNodeNumber(_tixihandle, xpath.c_str(), &nchildren);
  txutils::handle_error(res);
  if (res == FAILED) {
    nchildren = 0;
  }
  for (int i = 0; i < nchildren; i++) {
    QTX3Node* newNode = new QTX3Node(this);
    _children.append(newNode);
    newNode->createChildren();
  }
  return nchildren;
}

QString QTX3Node::xPath() const {
  if (!_parent) {
    // Then this is a root node
    return QString("/*[1]");
  }

  return _parent->xPath() + QString("/*[%1]").arg(QString::number(index() + 1));
}

QString QTX3Node::xmlPath() const {
  char* xmlpath;
  auto res = tixiXPathExpressionGetXPath(
      _tixihandle, xPath().toStdString().c_str(), 1, &xmlpath);
  if (res == SUCCESS) {
    return QString(xmlpath);
  } else if (res == FAILED) {
    // This shouldn't happen! Nodes and element count must agree
    throw std::runtime_error(std::string("Node ") + xPath().toStdString() +
                             std::string(" failed to resolve its own path!"));
  }
  //
  txutils::handle_error(res);
  return QString();
}

int QTX3Node::index() const {
  return _parent->_children.indexOf(this);
}

const QTX3Model* QTX3Node::model() const {
  return _model;
}
