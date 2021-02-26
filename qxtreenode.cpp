#include "qxtreenode.h"
#include "tixi_utils.h"

#include "xpathtreemodel.h"
QXTreeNode::QXTreeNode(QObject* parent) : QObject(parent) {
  // Try casting parent on either a model or node pointer
  XpathTreeModel* model = qobject_cast<XpathTreeModel*>(parent);
  _parent =
      qobject_cast<QXTreeNode*>(parent);  // If parent is not a node, but a
                                          // model, then this will be a nullptr
  if (model) {
    _model = model;
  } else if (_parent) {
    _model = _parent->_model;
  }

  _tixihandle = _model->_tixihandle;
}

QString QXTreeNode::xPath() const {
  if (!_parent) {
    // Then this is a root node
    return QString("/*[1]");
  }

  return _parent->xPath() + QString("/*[%1]").arg(QString::number(index() + 1));
}

QString QXTreeNode::xmlPath() {
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
  tixi_utils::handle_error(res);
  return QString();
}

int QXTreeNode::index() const {
  return _parent->_children.indexOf(this);
}
