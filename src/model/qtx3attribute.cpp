#include "qtx3attribute.h"
#include <QVariant>
#include "qtx3node.h"
using namespace QTX3;

Attribute::Attribute(Node* parent, const QString& attributeName)
    : Item(parent),
      _attributeName(strdup(attributeName.toStdString().c_str())) {}

QVariant Attribute::value() const {
  char* value;
  ReturnCode res = tixiCheckAttribute(
      _tixihandle, _parent->xPath().toStdString().c_str(), _attributeName);
  if (res == ATTRIBUTE_NOT_FOUND) {
    return QVariant();
  } else if (res == SUCCESS) {
    res = tixiGetTextAttribute(_tixihandle,
                               _parent->xPath().toStdString().c_str(),
                               _attributeName, &value);
  }
  if (res != SUCCESS)
    throw(std::runtime_error(
        QString(
            "QTX3Attribute::value(): failed to read attribute %1 at element %2")
            .arg(_attributeName)
            .arg(_parent->xmlPath())
            .toStdString()));

  return QVariant(value);
}
