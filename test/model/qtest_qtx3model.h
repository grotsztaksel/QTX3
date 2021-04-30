#ifndef QTX3MODELTEST_H
#define QTX3MODELTEST_H

#include <QObject>

namespace QTX3 {

class QTX3TestcaseModel;

class QTX3ModelTest : public QObject {
  Q_OBJECT
public:
private slots:

  void init();
  void cleanup();

  void test_constructor_with_tixihandle();
  void test_constructor_with_string();

  void test_index();
  void test_parent();

  void test_rowCount();
  void test_columnCount();

  void test_data_f(); // for some reason, test_data() is not recognized as test
  void test_setData();
  void test_setFlags();

  void test_addElement();
  void test_addElement2();
  void test_removeElement();

  void test_createNode_baseclass();
  // Using a specially modified model with a non-default factory
  void test_createNode_testclass();

  void test_nodeFromIndex();
  void test_nodeFromPath();
  void test_indexFromNode();

  void test_data();

  void test_reset();
  void test_tixi();

private:
  QTX3TestcaseModel *model;
};
} // namespace QTX3
#endif // QTX3MODELTEST_H
