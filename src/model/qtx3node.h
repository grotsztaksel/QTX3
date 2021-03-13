#ifndef QXTREENODE_H
#define QXTREENODE_H

#include <tixi.h>
#include <QObject>
#include <QVector>

#include "qtx3item.h"

class QTX3Model;
class QXT3NodeTest;
class QTX3Item;
class QTX3Node : public QObject {
  Q_OBJECT
 public:
  friend class QXT3NodeTest;
  friend QTX3Item::QTX3Item(QTX3Node* parent);
  explicit QTX3Node(QTX3Model* parent_model);
  explicit QTX3Node(QTX3Node* parent_node);

  // Recursively create children to adjust the node tree to the tixi XML
  // structure. Return number of children created on the top level
  int createChildren();

  // Return XML path in format '/*[i]/*[j]/*[k]...' associated with this node
  QString xPath() const;

  // Return XML path in format '/root/element/subelement[j]/thiselement'
  // associated with this node.
  QString xmlPath() const;

  // Return name of the own element
  QString elementName() const;

  QTX3Node* childAt(int row) const;
  int rows() const;

  // Return index of this node in the parent's _children
  int row() const;
  // Return number of columns this node produces
  virtual int columnCount() const;

  const QTX3Model* model() const;

  QTX3Node* parent() const;

  virtual QVariant data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const;

  /* set data through this item.
   * If operation is successful, a non-empty vector of roles is returned,
   * otherwise returns an empty vector. Returning a vector, instead of a bool
   * allows setting specific roles to emit dataChanged from the model.
   */
  virtual QVector<int> setData(const QModelIndex& index,
                               const QVariant& value,
                               int role = Qt::EditRole);

  virtual Qt::ItemFlags flags(const QModelIndex& index) const;

 signals:

 protected:
  // List of child elements
  QVector<QTX3Node*> _children;

  // List of items to be presented in the columns
  QVector<QTX3Item*> _columnItems;

  // Model to which the node belongs
  const QTX3Model* _model = nullptr;

  // Parent node. Can be retrieved by calling parent(), but would have to be
  // cast onto QXTreeNode every time
  QTX3Node* _parent = nullptr;

  TixiDocumentHandle _tixihandle;
};

#endif  // QXTREENODE_H
