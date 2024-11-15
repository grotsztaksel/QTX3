#ifndef QTX3MODEL_H
#define QTX3MODEL_H

#include "QTX3_global.h"
#include "qtx3node.h"
#include "tixi.h"
#include <QAbstractItemModel>

namespace QTX3 {

class QTX3_EXPORT Model : public QAbstractItemModel {
  Q_OBJECT

public:
  friend class QTX3ModelTest;
  friend Node::Node(Model *parent_model);
  friend void Node::createChildren(bool recursive);
  /*! @brief    Create the model with a tixihanlde given
   *  @warning  When subclassing this model, the initialization MUST be defered
   *            (use initialize == false)   */
  explicit Model(QObject *parent = nullptr, TixiDocumentHandle handle = -1,
                 bool initialize = true, int fetchmax = 100);
  explicit Model(QObject *parent = nullptr, const QString &rootName = "root",
                 bool initialize = true, int fetchmax = 100);

  /****************************************************
   *
   *
   *        Override QAbstractItemModel methods
   *
   *
   *****************************************************/

  // Basic functionality:
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  // Editable:
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole) override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  // Add data:
  Node *addElement(int row, const QString &name,
                   const QModelIndex &parent = QModelIndex());

  bool canFetchMore(const QModelIndex &parent) const override;

  void fetchMore(const QModelIndex &parent) override;

  // Remove data:
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;

  /****************************************************
   *
   *
   *        Class specific methods
   *
   *
   *****************************************************/

  // Reset the model using a new Tixi handle. If none (or invalid) is provided,
  // the model will be reset using its own tixi
  virtual void reset(const TixiDocumentHandle &newhandle = -1);

  // Access the model's tixi
  TixiDocumentHandle tixi() const;

  Node *nodeFromIndex(QModelIndex index = QModelIndex()) const;
  Node *nodeFromPath(QString path = "/*[1]");
  QModelIndex indexFromNode(Node *node) const;

protected:
  virtual void init();

  // Create an empty tixi element. Intended to be used in the constructor to
  // initialize an empty model from root element name.
  static TixiDocumentHandle createNewHandle(const QString &rootName);
  // Node factory
  virtual Node *createNode(Node *parent, const QString &name) const;

protected:
  TixiDocumentHandle tx = -1;
  const int maxNodesToFetch;

  Node *_root;
};
} // namespace QTX3
#endif // QTX3MODEL_H
