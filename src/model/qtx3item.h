#ifndef QTX3ITEM_H
#define QTX3ITEM_H

#include <QObject>
#include "tixi.h"

namespace QTX3 {

class QTX3Node;
class QTX3Model;
class QTX3Item : public QObject {
  Q_OBJECT
 public:
  explicit QTX3Item(QTX3Node* parent);

  // return row number
  int row() const;

  // return parent element node
  const QTX3Node* node() const;

  QTX3Node* parent() const;

 protected:
  QTX3Node* _parent;
  const QTX3Model* _model;
  const TixiDocumentHandle _tixihandle;
};
}  // namespace QTX3
#endif  // QTX3ITEM_H
