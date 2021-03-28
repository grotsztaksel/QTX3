#ifndef QTX3ATTRIBUTE_H
#define QTX3ATTRIBUTE_H

#include "QTX3_global.h"
#include "qtx3item.h"
#include <QObject>

namespace QTX3 {

/*
    Special Item class to handle XML attributes
*/
class QTX3_EXPORT Attribute : public Item {
  Q_OBJECT
public:
  Attribute(Node *parent, const QString &attributeName);

  QVariant value() const;

protected:
  const char *_attributeName;
};
} // namespace QTX3
#endif // QTX3ATTRIBUTE_H
