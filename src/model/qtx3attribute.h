#ifndef QTX3ATTRIBUTE_H
#define QTX3ATTRIBUTE_H

#include <QObject>
#include "qtx3item.h"

namespace QTX3 {

/*
    Special Item class to handle XML attributes
*/
class Attribute : public Item {
  Q_OBJECT
 public:
  Attribute(Node* parent, const QString& attributeName);

  QVariant value() const;

 protected:
  const char* _attributeName;
};
}  // namespace QTX3
#endif  // QTX3ATTRIBUTE_H
