#include "qtx3model.h"
#include "qtx3node.h"
#include "txutils.h"

#include <QRegularExpression>

using namespace QTX3;

Model::Model(QObject *parent, TixiDocumentHandle handle, bool initialize)
    : QAbstractItemModel(parent), _tixihandle(handle), _root(new Node(this)) {
  ReturnCode res = tixiCheckElement(handle, "/*[1]");
  if (res == INVALID_HANDLE) {
    throw(std::runtime_error(
        "QTX3Model: constructor received invalid tixi handle!"));
  }
  if (initialize)
    init();
}

Model::Model(QObject *parent, const QString &rootName, bool initialize)
    : Model(parent, Model::createNewHandle(rootName)) {}

void Model::init() { _root->createChildren(); }

TixiDocumentHandle Model::createNewHandle(const QString &rootName) {
  TixiDocumentHandle handle;
  ReturnCode res = tixiCreateDocument(rootName.toStdString().c_str(), &handle);
  if (res != SUCCESS) {
    throw(std::runtime_error(QString("QTX3Model: constructor failed to create "
                                     "tixihandle with root element \"%1\"")
                                 .arg(rootName)
                                 .toStdString()
                                 .c_str()));
  }
  return handle;
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  Node *parentNode;

  if (!parent.isValid())
    parentNode = _root;
  else
    parentNode = nodeFromIndex(parent);

  Node *childNode = parentNode->childAt(row);
  if (childNode)
    return createIndex(row, column, childNode);
  return QModelIndex();
}

QModelIndex Model::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return QModelIndex();

  Node *parentNode = nodeFromIndex(index)->parent();
  if (parentNode == _root)
    return QModelIndex();

  return createIndex(parentNode->row(), 0, parentNode);
}

int Model::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0)
    return 0;
  return nodeFromIndex(parent)->rows();
}

int Model::columnCount(const QModelIndex &parent) const {
  return nodeFromIndex(parent)->columnCount();
  /*
https://stackoverflow.com/questions/53219850/how-to-show-the-proper-number-of-columns-in-a-qtreeview-for-a-model-with-differe
    As @eyllanesc answered, the number of columns in the View is only affected
by the root item. However, columnCount() will have an effect on rows that give a
number that is less than this number, in that columns less than this numberwill
not be populated.
    */
}

QVariant Model::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  return nodeFromIndex(index)->data(index, role);
}

bool Model::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (data(index, role) != value) {
    QVector<int> roles = nodeFromIndex(index)->setData(index, value, role);
    if (roles.empty()) {
      return false;
    }
    emit dataChanged(index, index, roles);
    return true;
  }
  return false;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return nodeFromIndex(index)->flags(index);
}

bool Model::addElement(int row, const QString &name,
                       const QModelIndex &parent) {
  if (row < 0 || row > rowCount(parent))
    return false;
  auto parentNode = nodeFromIndex(parent);
  auto parentPath = parentNode->xPath();

  beginInsertRows(parent, row, row);
  ReturnCode res =
      tixiCreateElementAtIndex(_tixihandle, parentPath.toStdString().c_str(),
                               name.toStdString().c_str(), row + 1);

  if (res != SUCCESS) {
    return false;
  }
  auto newNode = createNode(parentNode, name);
  if (!newNode) {
    return false;
  }

  parentNode->insertChild(newNode, row);

  endInsertRows();
  return true;
}

bool Model::removeRows(int row, int count, const QModelIndex &parent) {
  auto parentNode = nodeFromIndex(parent);
  auto parentPath = parentNode->xPath();
  auto deletedPath = strdup(QString(parentPath + QString("/*[%1]").arg(row + 1))
                                .toStdString()
                                .c_str());
  beginRemoveRows(parent, row, row + count - 1);
  for (int i = 0; i < count; i++) {
    ReturnCode res = tixiRemoveElement(_tixihandle, deletedPath);
    if (res != SUCCESS) {
      return false;
    }
  }
  parentNode->removeChildren(row, count);
  endRemoveRows();
  return true;
}

void Model::reset(const TixiDocumentHandle &newhandle) {
  // first of all, check if the handle is valid
  ReturnCode res = tixiCheckElement(newhandle, "/*[1]");
  if (res == SUCCESS)
    _tixihandle = newhandle;

  beginResetModel();
  _root->deleteLater();
  _root = new Node(this);
  _root->createChildren();
  endResetModel();
}

TixiDocumentHandle Model::tixi() const { return _tixihandle; }

Node *Model::nodeFromIndex(QModelIndex index) const {
  if (!index.isValid())
    return _root;
  return static_cast<Node *>(index.internalPointer());
}

Node *Model::nodeFromPath(QString path) const {
  char *ipath;
  ReturnCode res =
      txutils::indexedPath(_tixihandle, path.toStdString().c_str(), 1, &ipath);
  if (res != SUCCESS)
    throw(std::runtime_error(QString("nodeFromPath: failed to convert path %1 "
                                     "to indexed XPath (TiXi error code: %2)")
                                 .arg(path)
                                 .arg(res)
                                 .toStdString()
                                 .c_str()));

  QRegularExpression re("(\\d+)");
  // Use size() to skip the part of the path refering to the root element.
  QRegularExpressionMatchIterator i =
      re.globalMatch(QString(ipath), QString("/*[1]").size());
  Node *node = _root;

  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    int number = match.captured().toInt() - 1;
    node = node->childAt(number);
  }
  return node;
}

Node *Model::createNode(Node *parent, const QString &name) const {
  // Basic implementation creates always the same nodes
  return new Node(parent);
}
