#ifndef QXTREENODE_H
#define QXTREENODE_H

#include <tixi.h>
#include <QObject>
#include <QVector>

class XpathTreeModel;

class QXTreeNode : public QObject {
  Q_OBJECT
 public:
  explicit QXTreeNode(QObject* parent = nullptr);

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

 signals:

 private:
  // List of child elements
  QVector<const QXTreeNode*> _children;

  // List of items to be presented in the columns
  QVector<QObject*> _columnItems;

  // Model to which the node belongs
  const XpathTreeModel* _model;

  // Parent node. Can be retrieved by calling parent(), but would have to be
  // cast onto QXTreeNode every time
  const QXTreeNode* _parent;

  TixiDocumentHandle _tixihandle;
};

#endif  // QXTREENODE_H
