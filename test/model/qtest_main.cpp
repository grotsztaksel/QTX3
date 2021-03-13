
#include "qtest_main.h"
#include <qlogging.h>
#include <QTest>
#include "qtest_qtx3item.h"
#include "qtest_qtx3model.h"
#include "qtest_qtx3node.h"

int main(int argc, char** argv) {
  QMap<QString, int> results;
  //-- run all tests
  {
    QTX3::QTX3ModelTest test_model;
    results["QTX3ModelTest"] = QTest::qExec(&test_model, argc, argv);
  }
  {
    QTX3::QTX3NodeTest test_node;
    results["QTX3NodeTest"] = QTest::qExec(&test_node, argc, argv);
  }
  {
    QTX3::QTX3ItemTest test_item;
    results["QTX3ItemTest"] = QTest::qExec(&test_item, argc, argv);
  }

  return printSummary(results);
}

int printSummary(QMap<QString, int> results) {
  QString title("All tests from all suites passed");
  QStringList failedSuites;
  int status = 0;
  for (auto key : results.keys()) {
    status |= results[key];
    if (results[key] != 0) {
      QString s;
      if (results[key] > 1)
        s = "s";
      failedSuites.append(
          QString("%1: failed %2 test%3").arg(key).arg(results[key]).arg(s));
    }
  }
  if (!failedSuites.empty()) {
    title = QString("%1 tests failed in %2 suites:\n")
                .arg(status)
                .arg(failedSuites.size());
  }
  qInfo().noquote() << title << failedSuites.join("\n");
}
