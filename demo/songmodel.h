#ifndef SONGMODEL_H
#define SONGMODEL_H

#include <QObject>
#include "qtx3model.h"

class SongModel : public QTX3Model {
  Q_OBJECT
 public:
  explicit SongModel(QObject* parent = nullptr,
                     TixiDocumentHandle handle = -1,
                     bool initialize = true);
  explicit SongModel(QObject* parent = nullptr,
                     const QString& rootName = "root",
                     bool initialize = true);

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

 protected:
  QTX3Node* createNode(QTX3Node* parent, const QString& name) const override;
};

#endif  // SONGMODEL_H
