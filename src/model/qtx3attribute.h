#ifndef QTX3ATTRIBUTE_H
#define QTX3ATTRIBUTE_H

#include <QObject>
#include "qtx3item.h"

namespace QTX3 {

/*
    Special Item class to handle XML attributes
*/
class QTX3Attribute : public QTX3Item {
  Q_OBJECT
 public:
  QTX3Attribute(QTX3Node* parent, const QString& attributeName);

  QVariant value() const;

 protected:
  const char* _attributeName;
};
}  // namespace QTX3
#endif  // QTX3ATTRIBUTE_H
