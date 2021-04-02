#ifndef QTX3NODE_H
#define QTX3NODE_H

#include "QTX3_global.h"
#include "tixi.h"
#include <QObject>
#include <QVector>

#include "qtx3item.h"

namespace QTX3 {

class Model;
class QTX3NodeTest;
class Item;
class QTX3_EXPORT Node : public QObject {
  Q_OBJECT
public:
  friend class QTX3NodeTest;
  friend class QTX3NodeGTest;
  friend Item::Item(Node *parent);
  explicit Node(Model *parent_model);
  explicit Node(Node *parent_node);

  // Recursively create children to adjust the node tree to the tixi XML
  // structure. Return number of children created on the top level
  void createChildren();

  // Return XML path in format '/*[i]/*[j]/*[k]...' associated with this node
  QString xPath() const;

  // Return XML path in format '/root/element/subelement[j]/thiselement'
  // associated with this node.
  QString xmlPath() const;

  // Return name of the own element
  QString elementName() const;

  Node *childAt(int row) const;

  void insertChild(Node *child, const int index);

  void removeChildren(const int first, const int count);

  int rows() const;

  // Return index of this node in the parent's _children
  int row() const;
  // Return number of columns this node produces
  virtual int columnCount() const;

  Model *model() const;

  Node *parent() const;

  virtual QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const;

  /* set data through this item.
   * If operation is successful, a non-empty vector of roles is returned,
   * otherwise returns an empty vector. Returning a vector, instead of a bool
   * allows setting specific roles to emit dataChanged from the model.
   */
  virtual QVector<int> setData(const QModelIndex &index, const QVariant &value,
                               int role = Qt::EditRole);

  virtual Qt::ItemFlags flags(const QModelIndex &index) const;

signals:

protected:
  // List of child elements
  QVector<Node *> _children = QVector<Node *>();

  // List of items to be presented in the columns
  QVector<Item *> _columnItems = QVector<Item *>();

  // Model to which the node belongs
  const Model *_model = nullptr;

  // Parent node. Can be retrieved by calling parent(), but would have to be
  // cast onto QTX3Node every time
  Node *_parent = nullptr;

  TixiDocumentHandle *_tixihandle;
};
} // namespace QTX3
#endif // QTX3NODE_H
