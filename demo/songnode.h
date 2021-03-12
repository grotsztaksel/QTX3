#ifndef SONGNODE_H
#define SONGNODE_H

#include <qtx3node.h>
#include <QObject>

class SongNode : public QTX3Node {
  Q_OBJECT
 public:
  SongNode(QTX3Node* parent_node);

  int columnCount() const override;
  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;
};

#endif  // SONGNODE_H
