#ifndef SONGNODE_H
#define SONGNODE_H

#include <qtx3node.h>
#include <QObject>

class SongNode : public QTX3Node {
  Q_OBJECT
 public:
  SongNode(QTX3Node* parent_node);
};

#endif  // SONGNODE_H
