#ifndef QTX3ITEMTEST_H
#define QTX3ITEMTEST_H

#include <QObject>

namespace QTX3 {

class Model;
class QTX3specialTestNode;
class Item;

class QTX3ItemTest : public QObject {
  Q_OBJECT
public:
private slots:
  void init();
  void cleanup();

  void test_row();
  void test_parent();
  void test_node();

private:
  Model *model;
  QTX3specialTestNode *node;
};
} // namespace QTX3
#endif // QTX3ITEMTEST_H
