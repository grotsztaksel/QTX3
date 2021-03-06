#ifndef QTX3TESTCASEMODEL_H
#define QTX3TESTCASEMODEL_H

#include <QObject>
#include "qtx3model.h"


/***************************************************
 *
 *
 *    Special model used to test the nodes
 *
 *
 ***************************************************/


class QTX3TestcaseModel: public QTX3Model
{
    Q_OBJECT
public:
    QTX3TestcaseModel(QObject* parent=nullptr);
};

#endif // QTX3TESTCASEMODEL_H

