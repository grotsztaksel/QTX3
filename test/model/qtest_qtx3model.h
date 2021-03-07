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

 private:
  QTX3TestcaseModel* model;
};

#endif  // QTX3MODELTEST_H
