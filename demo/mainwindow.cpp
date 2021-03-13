#include "mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "./ui_mainwindow.h"
#include "tixi.h"

#include "songmodel.h"

using namespace QTX3;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionOpen_file, SIGNAL(triggered()), this,
          SLOT(onOpenFileTriggered()));
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::onOpenFileTriggered() {
  QString xmlFileName;
  bool use_dialog = false;
  if (use_dialog) {
    xmlFileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                               tr("XML files (*.xml)"));
  } else {
    xmlFileName =
        "C:/Users/piotr/Documents/Songs XML/SeparateFiles/0_main_file.xml";
  }

  TixiDocumentHandle handle;
  ReturnCode res = tixiOpenDocument(xmlFileName.toStdString().c_str(), &handle);
  if (res != SUCCESS) {
    QMessageBox::critical(this, "File open error",
                          QString("Could not open %1!\nTiXi error code: %2")
                              .arg(xmlFileName)
                              .arg(res));
    return;
  }

  Model* model = new SongModel(ui->treeView, handle);

  ui->treeView->setModel(model);
  char* text;
  res = tixiExportDocumentAsString(handle, &text);
  if (res == SUCCESS)
    ui->textView->document()->setPlainText(QString(text));
}
