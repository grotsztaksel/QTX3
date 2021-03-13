#ifndef QTX3ITEM_H
#define QTX3ITEM_H

#include <QObject>
#include "tixi.h"

namespace QTX3 {

class Node;
class Model;
class Item : public QObject {
  Q_OBJECT
 public:
  explicit Item(Node* parent);

  // return row number
  int row() const;

  // return parent element node
  const Node* node() const;

  Node* parent() const;

 protected:
  Node* _parent;
  const Model* _model;
  const TixiDocumentHandle _tixihandle;
};
}  // namespace QTX3
#endif  // QTX3ITEM_H
