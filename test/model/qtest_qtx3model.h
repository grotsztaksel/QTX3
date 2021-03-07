#ifndef QTX3MODELTEST_H
#define QTX3MODELTEST_H

#include <QObject>
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

  void test_createNode_baseclass();
  // Using a specially modified model with a non-default factory
  void test_createNode_testclass();

 private:
  QTX3TestcaseModel* model;
};

#endif  // QTX3MODELTEST_H
