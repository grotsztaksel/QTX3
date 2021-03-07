#ifndef QTX3TESTCASEMODEL_H
#define QTX3TESTCASEMODEL_H

#include <QObject>
#include "qtx3model.h"
#include "tixi.h"

/***************************************************
 *
 *
 *    Special model used to test the nodes
 *
 *
 ***************************************************/

class QTX3TestcaseModel : public QTX3Model {
  Q_OBJECT
 public:
  QTX3TestcaseModel(QObject* parent = nullptr, TixiDocumentHandle handle = -1);

  static QTX3TestcaseModel* createModel(QObject* parent);
};

#endif  // QTX3TESTCASEMODEL_H
