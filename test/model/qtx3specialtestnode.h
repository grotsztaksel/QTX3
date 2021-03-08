#ifndef QTX3SPECIALTESTNODE_H
#define QTX3SPECIALTESTNODE_H

#include <QObject>
#include "qtx3node.h"

class QTX3specialTestNode : public QTX3Node {
  Q_OBJECT
 public:
  friend class QTX3ModelTest;
  explicit QTX3specialTestNode(QTX3Node* parent_node);

 protected:
  void createItems();
 signals:
};

#endif  // QTX3SPECIALTESTNODE_H
