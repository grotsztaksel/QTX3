#ifndef QTX3SPECIALTESTNODE_H
#define QTX3SPECIALTESTNODE_H

#include <QObject>
#include "qtx3node.h"

namespace QTX3 {

class QTX3specialTestNode : public QTX3Node {
  Q_OBJECT
 public:
  friend class QTX3ModelTest;
  explicit QTX3specialTestNode(QTX3Node* parent_node);

  int columnCount() const override;

 protected:
 signals:
};
}  // namespace QTX3
#endif  // QTX3SPECIALTESTNODE_H
