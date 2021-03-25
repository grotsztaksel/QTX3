#ifndef QTX3ATTRIBUTETEST_H
#define QTX3ATTRIBUTETEST_H

#include "tixi.h"
#include <QObject>

namespace QTX3 {

class Model;

class QTX3AttributeTest : public QObject {
  Q_OBJECT
public:
private slots:
  void init();
  void cleanup();

  void test_value();

private:
  TixiDocumentHandle handle;
  Model *model = nullptr;
};
} // namespace QTX3
#endif // QTX3ATTRIBUTETEST_H
