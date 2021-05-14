#include "qtx3node.h"
#include "txutils.h"

#include "qtx3model.h"

using namespace QTX3;

Node::Node(Model *parent_model)
    : QObject(parent_model), _model(parent_model),
      tx(&parent_model->tx) {}

Node::Node(Node *parent_node)
    : QObject(parent_node), _model(parent_node->_model), _parent(parent_node),
      tx(parent_node->tx) {}

void Node::createChildren() {
  if (_children.size() > 0)
    _children.clear();

  std::string xpath_s = xPath().toStdString() + "/*";
  const char *xpath = xpath_s.c_str();
  int nchildren = 0;
  auto res = txutils::expectCode(
      tixiXPathEvaluateNodeNumber(*tx, xpath, &nchildren), {},
      {SUCCESS, FAILED});
  if (res == FAILED) {
    return;
  }
  for (int i = 1; i <= nchildren; i++) {
    char *newPath;
    res = tixiXPathExpressionGetXPath(*tx, xpath, i, &newPath);
    Node *newNode =
        _model->createNode(this, QString(txutils::elementName(newPath)));
    _children.append(newNode);
    newNode->createChildren();
  }
}

QString Node::xPath() const {
  if (!_parent) {
    // Then this is a root node
    return QString("/*[1]");
  }

  return _parent->xPath() + QString("/*[%1]").arg(QString::number(row() + 1));
}

QString Node::xmlPath() const {
  char *xmlpath;
  ReturnCode res = txutils::excludeCode(
      tixiXPathExpressionGetXPath(*tx, xPath().toStdString().c_str(),
                                  1, &xmlpath),
      std::string("Node ") + xPath().toStdString() +
          std::string(" failed to resolve its own path!"));

  if (res == SUCCESS) {
    return QString(xmlpath);
  }
  return QString();
}

QString Node::elementName() const {
  return QString(txutils::elementName(xmlPath().toStdString().c_str()));
}

Node *Node::childAt(int row) const { return _children.at(row); }

void Node::insertChild(Node *child, const int index) {
  _children.insert(index, child);
}

void Node::removeChildren(const int first, const int count) {
  for (int i = first; i < first + count; i++) {
    delete _children.at(i);
  }
  _children.remove(first, count);
}

int Node::rows() const { return _children.size(); }

int Node::row() const {
  if (!_parent)
    return 0;
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

int Node::columnCount() const { return 1; }

Model *Node::model() const { return const_cast<Model *>(_model); }

Node *Node::parent() const { return _parent; }

QModelIndex Node::index() { return model()->indexFromNode(this); }

QVariant Node::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole && index.column() == 0) {
    // Default implementation displays element name
    return QVariant(elementName());
  }
  return QVariant();
}

QVector<int> Node::setData(const QModelIndex &index, const QVariant &value,
                           int role) {
  // Base class does not allow editing
  return QVector<int>();
}

Qt::ItemFlags Node::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;
  auto flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (index.column() != 0)
    flags | Qt::ItemNeverHasChildren;
  return flags;
}

QVector<Node *> Node::childNodes() const { return _children; }
