#ifndef QTX3SPECIALTESTNODE_H
#define QTX3SPECIALTESTNODE_H

#include <QObject>
#include "qtx3node.h"

namespace QTX3 {

class Item;

class QTX3specialTestNode : public Node {
  Q_OBJECT
 public:
  friend class QTX3ModelTest;

  explicit QTX3specialTestNode(Node* parent_node);

  int columnCount() const override;

  const Item* item() const;

 protected:
  const Item* _item;
};
}  // namespace QTX3
#endif  // QTX3SPECIALTESTNODE_H
