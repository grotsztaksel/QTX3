#ifndef QTX3TESTCASEMODEL_H
#define QTX3TESTCASEMODEL_H

#include "qtx3model.h"
#include "tixi.h"
#include <QObject>

/***************************************************
 *
 *
 *    Special model used to test the nodes
 *
 *
 ***************************************************/

namespace QTX3 {

class QTX3TestcaseModel : public Model {
  Q_OBJECT
public:
  friend class QTX3ModelTest;
  QTX3TestcaseModel(QObject *parent = nullptr, TixiDocumentHandle handle = -1,
                    bool initialize = true);

  static QTX3TestcaseModel *createModel(QObject *parent);

protected:
  Node *createNode(Node *parent, const QString &name) const;
};
} // namespace QTX3
#endif // QTX3TESTCASEMODEL_H
