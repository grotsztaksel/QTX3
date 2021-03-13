#ifndef QTX3NODETEST_H
#define QTX3NODETEST_H

#include <QObject>

namespace QTX3 {

class QTX3TestcaseModel;
class Node;
class QTX3NodeTest : public QObject {
  Q_OBJECT
 public:
 private slots:
  void init();
  void cleanup();

  void test_rows();
  void test_columnCount();
  void test_row();
  void test_xPath();
  void test_xmlPath();
  void test_elementName();

  void test_model();
  void test_parent();

 private:
  QTX3TestcaseModel* model;
  Node* rootNode;
  Node* node;
};
}  // namespace QTX3
#endif  // QTX3NODETEST_H
