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

  QTX3Node* parentNode;

  if (!parent.isValid())
    parentNode = _root;
  else
    parentNode = nodeFromIndex(parent);

  QTX3Node* childNode = parentNode->childAt(row);
  if (childNode)
    return createIndex(row, column, childNode);
  return QModelIndex();
}

QModelIndex QTX3Model::parent(const QModelIndex& index) const {
  if (!index.isValid())
    return QModelIndex();

  QTX3Node* parentNode = nodeFromIndex(index)->parent();
  if (parentNode == _root)
    return QModelIndex();

  return createIndex(parentNode->row(), 0, parentNode);
}

int QTX3Model::rowCount(const QModelIndex& parent) const {
  if (parent.column() > 0)
    return 0;
  return nodeFromIndex(parent)->rows();
}

int QTX3Model::columnCount(const QModelIndex& parent) const {
  return nodeFromIndex(parent)->columnCount();
  /*
https://stackoverflow.com/questions/53219850/how-to-show-the-proper-number-of-columns-in-a-qtreeview-for-a-model-with-differe
    As @eyllanesc answered, the number of columns in the View is only affected
by the root item. However, columnCount() will have an effect on rows that give a
number that is less than this number, in that columns less than this numberwill
not be populated.
    */
}

QVariant QTX3Model::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();

  return nodeFromIndex(index)->data(index, role);
}

bool QTX3Model::setData(const QModelIndex& index,
                        const QVariant& value,
                        int role) {
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

Qt::ItemFlags QTX3Model::flags(const QModelIndex& index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return nodeFromIndex(index)->flags(index);
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

QTX3Node* QTX3Model::nodeFromIndex(QModelIndex index) const {
  if (!index.isValid())
    return _root;
  return static_cast<QTX3Node*>(index.internalPointer());
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
