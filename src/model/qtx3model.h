#ifndef QTX3MODEL_H
#define QTX3MODEL_H

#include <QAbstractItemModel>
#include "tixi.h"

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
  bool addElement(int row,
                  const QString& name,
                  const QModelIndex& parent = QModelIndex());

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
#endif  // QTX3MODEL_H
