#include "qtx3node.h"
#include "txutils.h"

#include "qtx3model.h"
QTX3Node::QTX3Node(QTX3Model* parent_model)
    : QObject(parent_model),
      _model(parent_model),
      _tixihandle(parent_model->_tixihandle) {
  ;
}

QTX3Node::QTX3Node(QTX3Node* parent_node)
    : QObject(parent_node),
      _model(parent_node->_model),
      _parent(parent_node),
      _tixihandle(parent_node->_tixihandle) {}

int QTX3Node::createChildren() {
  if (_children.size() > 0)
    // Already has children. Should use insert/remove rows to add items.
    return 0;

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

  return _parent->xPath() + QString("/*[%1]").arg(QString::number(row() + 1));
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

QString QTX3Node::elementName() const {
  return QString(txutils::elementName(xmlPath().toStdString().c_str()));
}

QTX3Item* QTX3Node::itemAt(int column) const {
  return _columnItems.at(column);
}

QTX3Node* QTX3Node::childAt(int row) const {
  return _children.at(row);
}

int QTX3Node::rows() const {
  return _children.size();
}

int QTX3Node::row() const {
  for (int i = 0; i < _parent->_children.size(); i++) {
    if (_parent->_children.at(i) == this)
      return i;
  }
  return -1;
  /* Cannot use the following:
      return _parent->_children.indexOf(this);
    because it is illegal:
    * error: reference to type 'QTX3Node *const' could not bind to an rvalue of
    type 'const QTX3Node *'
    * error: invalid conversion from 'const QTX3Node*' to 'QTX3Node*'
    [-fpermissive]
  */
}

const QTX3Model* QTX3Node::model() const {
  return _model;
}

const QTX3Node* QTX3Node::parent() const {
  return _parent;
}
