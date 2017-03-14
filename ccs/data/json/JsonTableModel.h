#ifndef JSONTABLEMODEL_H
#define JSONTABLEMODEL_H

#include <QAbstractItemModel>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QIcon>

class JsonTableModel;

class JsonTreeItem {
public:
    JsonTreeItem(JsonTreeItem * parent = 0);
    ~JsonTreeItem();
    void appendChild(JsonTreeItem * item);
    JsonTreeItem *child(int row);
    JsonTreeItem *parent();
    int childCount() const;
    int row() const;
    void setKey(const QString& key);
    void setValue(const QString& value);
    void setType(const QJsonValue::Type& type);
    QString key() const;
    QString value() const;
    QJsonValue::Type type() const;


    static JsonTreeItem* load(const QJsonValue& value,
                              JsonTreeItem * parent = 0);

protected:


private:
    QString mKey;
    QString mValue;
    QJsonValue::Type mType;
    QList<JsonTreeItem*> mChilds;
    JsonTreeItem * mParent;


};

class JsonTableModel :
        public QAbstractItemModel {
    Q_OBJECT
public:
    explicit JsonTableModel(QObject *parent = 0);
    bool load(const QString& fileName);
    bool load(QIODevice * device);
    bool loadJson(const QByteArray& json);
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    JsonTreeItem * mRootItem;
    QJsonDocument mDocument;
    QStringList mHeaders;


};
#endif // JSONTABLEMODEL_H
