#include "songmodel.h"
#include "songnode.h"

#include <QColor>

SongModel::SongModel(QObject* parent,
                     TixiDocumentHandle handle,
                     bool initialize)
    : Model(parent, handle, false) {
  if (initialize)
    init();
}

SongModel::SongModel(QObject* parent, const QString& rootName, bool initialize)
    : Model(parent, rootName, initialize) {}

QVariant SongModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::ForegroundRole &&
      nodeFromIndex(index)->elementName() == "song")
    return QVariant(QColor(Qt::darkGreen));

  return Model::data(index, role);
}

int SongModel::columnCount(const QModelIndex& parent) const {
  if (nodeFromIndex(parent)->elementName() == "section") {
    return 2;
  }
  return 3;
}

QTX3::Node* SongModel::createNode(QTX3::Node* parent,
                                      const QString& name) const {
  // Basic implementation creates always the same nodes
  if (name == "song") {
    return new SongNode(parent);
  }
  return QTX3::Model::createNode(parent, name);
}
