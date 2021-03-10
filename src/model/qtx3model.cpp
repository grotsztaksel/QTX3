#include "qtx3model.h"
#include "qtx3node.h"
#include "txutils.h"

#include <QRegularExpression>

QTX3Model::QTX3Model(QObject* parent,
                     TixiDocumentHandle handle,
                     bool initialize)
    : QAbstractItemModel(parent),
      _tixihandle(handle),
      _root(new QTX3Node(this)) {
  if (handle <= 0) {
    throw(std::runtime_error(
        "QTX3Model: constructor received invalid tixi handle!"));
  }
  if (initialize)
    init();
}

QTX3Model::QTX3Model(QObject* parent, const QString& rootName, bool initialize)
    : QTX3Model(parent, QTX3Model::createNewHandle(rootName)) {}

void QTX3Model::init() {
  _root->createChildren();
}

TixiDocumentHandle QTX3Model::createNewHandle(const QString& rootName) {
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

QVariant QTX3Model::headerData(int section,
                               Qt::Orientation orientation,
                               int role) const {
  if (role == Qt::DisplayRole) {
    return QVariant("element");
  }
  return QVariant();
}

bool QTX3Model::setHeaderData(int section,
                              Qt::Orientation orientation,
                              const QVariant& value,
                              int role) {
  if (value != headerData(section, orientation, role)) {
    // FIXME: Implement me!
    emit headerDataChanged(orientation, section, section);
    return true;
  }
  return false;
}

QModelIndex QTX3Model::index(int row,
                             int column,
                             const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent))
    return QModelIndex();
  const QTX3Node* parentNode;
  if (!parent.isValid()) {
    parentNode = _root;
  } else {
    parentNode = nodeFromIndex(parent);
  }
  QTX3Node* childNode = parentNode->childAt(row);
  if (!childNode)
    return QModelIndex();

  QTX3Item* columnItem = childNode->itemAt(column);

  return createIndex(row, column, columnItem);
}

QModelIndex QTX3Model::parent(const QModelIndex& index) const {
  if (!index.isValid())
    return QModelIndex();
  const QTX3Node* parentNode = nodeFromIndex(index)->parent();
  return createIndex(parentNode->row(), 0, parentNode->itemAt(0));
}

int QTX3Model::rowCount(const QModelIndex& parent) const {
  if (parent.column() > 0)
    return 0;
  return nodeFromIndex(parent)->rows();
}

int QTX3Model::columnCount(const QModelIndex& parent) const {
  return 1;
  int ic = 0;
  auto parent_node = nodeFromIndex(parent);
  // This may be sufficient. The question is what "parent" means
  //  return parent_node->columns();

  for (int i = 0; i < parent_node->rows(); i++) {
    int child_columnCount = parent_node->childAt(i)->columns();

    ic = child_columnCount > ic ? child_columnCount : ic;
  }
  return ic;
}

QVariant QTX3Model::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();

  return itemFromIndex(index)->data(role);
}

bool QTX3Model::setData(const QModelIndex& index,
                        const QVariant& value,
                        int role) {
  if (data(index, role) != value) {
    QVector<int> roles = itemFromIndex(index)->setData(value, role);
    if (roles.empty()) {
      return false;
    }
    emit dataChanged(index, index, roles);
    return true;
  }
  return false;
}

Qt::ItemFlags QTX3Model::flags(const QModelIndex& index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return itemFromIndex(index)->flags();
}

bool QTX3Model::insertRows(int row, int count, const QModelIndex& parent) {
  return false;
  beginInsertRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endInsertRows();
}

bool QTX3Model::removeRows(int row, int count, const QModelIndex& parent) {
  return false;
  beginRemoveRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endRemoveRows();
}

QTX3Item* QTX3Model::itemFromIndex(QModelIndex index) const {
  return static_cast<QTX3Item*>(index.internalPointer());
}

QTX3Node* QTX3Model::nodeFromIndex(QModelIndex index) const {
  if (!index.isValid())
    return _root;
  return itemFromIndex(index)->parent();
}

QTX3Node* QTX3Model::nodeFromPath(QString path) const {
  char* ipath;
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
  QTX3Node* node = _root;

  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    int number = match.captured().toInt() - 1;
    node = node->childAt(number);
  }
  return node;
}

QTX3Node* QTX3Model::createNode(QTX3Node* parent, const QString& name) const {
  // Basic implementation creates always the same nodes
  return new QTX3Node(parent);
}
