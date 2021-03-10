
#include <QTest>

#include "qtest_qtx3node.h"
#include "qtest_qtx3model.h"

int main(int argc, char **argv)
{
   int status = 0;

   //-- run all tests
   {
      QTX3NodeTest test_node;
      status |= QTest::qExec(&test_node, argc, argv);
   }

   {
      QTX3ModelTest test_model;
      status |= QTest::qExec(&test_model, argc, argv);
   }

   return status;
}