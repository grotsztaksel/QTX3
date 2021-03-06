#ifndef QXTREENODE_H
#define QXTREENODE_H

#include <tixi.h>
#include <QObject>
#include <QVector>

class QTX3Model;

class QTX3Node : public QObject {
  Q_OBJECT
 public:
  friend class QTX3Model;
  explicit QTX3Node(QTX3Model* parent_model);
  explicit QTX3Node(QTX3Node* parent_node);

  // Recursively create children to adjust the node tree to the tixi XML
  // structure
  void createChildren();

  // Return XML path in format '/*[i]/*[j]/*[k]...' associated with this node
  QString xPath() const;

  // Return XML path in format '/root/element/subelement[j]/thiselement'
  // associated with this node.
  QString xmlPath();

  // Return index of this node in the parent's _children
  int index() const;

  const QTX3Model* model() const;

 signals:

 private:
  // List of child elements
  QVector<const QTX3Node*> _children;

  // List of items to be presented in the columns
  QVector<QObject*> _columnItems;

  // Model to which the node belongs
  const QTX3Model* _model = nullptr;

  // Parent node. Can be retrieved by calling parent(), but would have to be
  // cast onto QXTreeNode every time
  const QTX3Node* _parent = nullptr;

  TixiDocumentHandle _tixihandle;
};

#endif  // QXTREENODE_H
