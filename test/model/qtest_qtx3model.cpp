#include <QObject>

class QTX3ModelTest : public QObject {
  Q_OBJECT
 public:
  explicit QTX3ModelTest(QObject* parent = nullptr);

 signals:
};
QTX3ModelTest::QTX3ModelTest(QObject* parent) : QObject(parent) {}
