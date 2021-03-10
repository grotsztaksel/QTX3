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

  virtual QVariant data(int role) const;

  /* set data through this item.
   * If operation is successful, a non-empty vector of roles is returned,
   * otherwise returns an empty vector. Returning a vector, instead of a bool
   * allows setting specific roles to emit dataChanged from the model.
   */
  virtual QVector<int> setData(QVariant value, int role);

  virtual Qt::ItemFlags flags() const;

  QTX3Node* parent() const;

 private:
  QTX3Node* _parent;
  const QTX3Model* _model;
  const TixiDocumentHandle _tixihandle;
};

#endif  // QTX3ITEM_H
