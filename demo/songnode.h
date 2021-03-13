#ifndef SONGNODE_H
#define SONGNODE_H

#include <qtx3node.h>
#include <QObject>
#include "qtx3attribute.h"

class SongNode : public QTX3::Node {
  Q_OBJECT
 public:
  SongNode(QTX3::Node* parent_node);

  int columnCount() const override;
  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;

 protected:
  const QTX3::Attribute _title;
};

#endif  // SONGNODE_H
