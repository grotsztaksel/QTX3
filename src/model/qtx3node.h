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
  friend int QTX3Item::column() const;
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

  QTX3Item* itemAt(int column) const;
  int columns() const;
  QTX3Node* childAt(int row) const;
  int rows() const;

  // Return index of this node in the parent's _children
  int row() const;

  const QTX3Model* model() const;

  QTX3Node* parent() const;

 protected:
  // Fill the list _columnItems. The default implementation creates one item
  virtual void createItems();
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
