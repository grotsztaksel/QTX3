#include "songmodel.h"
#include "songnode.h"

#include <QColor>

SongModel::SongModel(QObject* parent,
                     TixiDocumentHandle handle,
                     bool initialize)
    : QTX3Model(parent, handle, initialize) {}

SongModel::SongModel(QObject* parent, const QString& rootName, bool initialize)
    : QTX3Model(parent, rootName, initialize) {}

QVariant SongModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::ForegroundRole &&
      nodeFromIndex(index)->elementName() == "song")
    return QVariant(QColor(Qt::darkGreen));

  return QTX3Model::data(index, role);
}

QTX3Node* SongModel::createNode(QTX3Node* parent, const QString& name) const {
  // Basic implementation creates always the same nodes
  if (name == "song") {
    return new SongNode(parent);
  }
  return QTX3Model::createNode(parent, name);
}
