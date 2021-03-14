#include "treeview.h"
#include <QKeyEvent>
#include "songmodel.h"

#include <QApplication>
#include <QEvent>
#include <iostream>
TreeView::TreeView(QWidget* parent) : QTreeView(parent) {}

void TreeView::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Delete) {
    QModelIndex index = currentIndex();
    model()->removeRows(index.row(), 1, index.parent());
  } else if (event->key() == Qt::Key_Plus &&
             event->modifiers() & Qt::ShiftModifier) {
    addRenameElement();
  }
}

void TreeView::addRenameElement() {
  QModelIndex index = currentIndex();
  auto smodel = qobject_cast<SongModel*>(model());
  smodel->addElement(index.row() + 1, "new", index.parent());
}
