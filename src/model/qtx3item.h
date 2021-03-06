#ifndef QTX3ITEM_H
#define QTX3ITEM_H

#include <QObject>
#include "tixi.h"
class QTX3Node;
class QTX3Model;
class QTX3Item : public QObject {
  Q_OBJECT
 public:
  explicit QTX3Item(QTX3Node* parent);

  // return column number
  int column() const;

  // return row number
  int row() const;

  // return parent element node
  const QTX3Node* node() const;

  QVariant data(Qt::ItemDataRole role);

  bool setData(QVariant value, Qt::ItemDataRole role);

  Qt::ItemFlags flags();

 private:
  TixiDocumentHandle getTixiHandle();

 private:
  const QTX3Node* _parent;
  const QTX3Model* _model;
  const TixiDocumentHandle _tixihandle;
};

#endif  // QTX3ITEM_H
