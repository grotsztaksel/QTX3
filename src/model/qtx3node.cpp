#include "qtx3node.h"
#include "txutils.h"

#include "qtx3model.h"
QTX3Node::QTX3Node(QObject* parent) : QObject(parent) {
  // Try casting parent on either a model or node pointer
  QTX3Model* model = qobject_cast<QTX3Model*>(parent);
  _parent =
      qobject_cast<QTX3Node*>(parent);  // If parent is not a node, but a
                                          // model, then this will be a nullptr
  if (model) {
    _model = model;
  } else if (_parent) {
    _model = _parent->_model;
  }

  _tixihandle = _model->_tixihandle;
}

void QTX3Node::createChildren() {
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
}

QString QTX3Node::xPath() const {
  if (!_parent) {
    // Then this is a root node
    return QString("/*[1]");
  }

  return _parent->xPath() + QString("/*[%1]").arg(QString::number(index() + 1));
}

QString QTX3Node::xmlPath() {
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
