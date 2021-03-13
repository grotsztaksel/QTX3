#ifndef XPATHTREEMODEL_H
#define XPATHTREEMODEL_H

#include <tixi.h>
#include <QAbstractItemModel>

#include "qtx3node.h"

namespace QTX3 {

class Model : public QAbstractItemModel {
  Q_OBJECT

 public:
  friend class QTX3ModelTest;
  friend Node::Node(Model* parent_model);
  friend int Node::createChildren();
  explicit Model(QObject* parent = nullptr,
                     TixiDocumentHandle handle = -1,
                     bool initialize = true);
  explicit Model(QObject* parent = nullptr,
                     const QString& rootName = "root",
                     bool initialize = true);

  /****************************************************
   *
   *
   *        Override QAbstractItemModel methods
   *
   *
   *****************************************************/
  // Header:
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  bool setHeaderData(int section,
                     Qt::Orientation orientation,
                     const QVariant& value,
                     int role = Qt::EditRole) override;

  // Basic functionality:
  QModelIndex index(int row,
                    int column,
                    const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;

  // Editable:
  bool setData(const QModelIndex& index,
               const QVariant& value,
               int role = Qt::EditRole) override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;

  // Add data:
  bool insertRows(int row,
                  int count,
                  const QModelIndex& parent = QModelIndex()) override;

  // Remove data:
  bool removeRows(int row,
                  int count,
                  const QModelIndex& parent = QModelIndex()) override;

  /****************************************************
   *
   *
   *        Class specific methods
   *
   *
   *****************************************************/

  Node* nodeFromIndex(QModelIndex index = QModelIndex()) const;
  Node* nodeFromPath(QString path = "/*[1]") const;

 protected:
  virtual void init();

  // Create an empty tixi element. Intended to be used in the constructor to
  // initialize an empty model from root element name.
  static TixiDocumentHandle createNewHandle(const QString& rootName);
  // Node factory
  virtual Node* createNode(Node* parent, const QString& name) const;

 protected:
  const TixiDocumentHandle _tixihandle = -1;

  Node* _root;
};
}  // namespace QTX3
#endif  // XPATHTREEMODEL_H
