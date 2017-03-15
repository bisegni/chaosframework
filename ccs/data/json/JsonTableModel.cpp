#include "JsonTableModel.h"
#include <QFile>
#include <QDebug>
#include <QFont>


JsonTreeItem::JsonTreeItem(JsonTreeItem *parent) {
    mParent = parent;
}

JsonTreeItem::~JsonTreeItem() {
    qDeleteAll(mChilds);
}

void JsonTreeItem::appendChild(JsonTreeItem *item) {
    mChilds.append(item);
}

JsonTreeItem *JsonTreeItem::child(int row) {
    return mChilds.value(row);
}

JsonTreeItem *JsonTreeItem::parent() {
    return mParent;
}

int JsonTreeItem::childCount() const {
    return mChilds.count();
}

int JsonTreeItem::row() const {
    if (mParent) {
        return mParent->mChilds.indexOf(const_cast<JsonTreeItem*>(this));
    }
    return 0;
}

void JsonTreeItem::setKey(const QString &key) {
    mKey = key;
}

void JsonTreeItem::setValue(const QString &value) {
    mValue = value;
}

void JsonTreeItem::setType(const QJsonValue::Type &type) {
    mType = type;
}

QString JsonTreeItem::key() const {
    return mKey;
}

QString JsonTreeItem::value() const {
    return mValue;
}

QJsonValue::Type JsonTreeItem::type() const {
    return mType;
}

JsonTreeItem* JsonTreeItem::load(const QJsonValue& value,
                                 JsonTreeItem* parent) {
    JsonTreeItem * rootItem = new JsonTreeItem(parent);
    rootItem->setKey("root");
    if ( value.isObject()) {

        //Get all QJsonValue childs
        for (QString key : value.toObject().keys()){
            QJsonValue v = value.toObject().value(key);
            JsonTreeItem * child = load(v,rootItem);
            child->setKey(key);
            child->setType(v.type());
            rootItem->appendChild(child);

        }

    } else if ( value.isArray()) {
        //Get all QJsonValue childs
        int index = 0;
        for (QJsonValue v : value.toArray()){

            JsonTreeItem * child = load(v,rootItem);
            child->setKey(QString::number(index));
            child->setType(v.type());
            rootItem->appendChild(child);
            ++index;
        }
    } else {
        rootItem->setValue(value.toVariant().toString());
        rootItem->setType(value.type());
    }

    return rootItem;
}

//=========================================================================

JsonTableModel::JsonTableModel(QObject *parent) :
    QAbstractItemModel(parent) {
    root_item = new JsonTreeItem();
    mHeaders.append("key");
    mHeaders.append("value");


}

bool JsonTableModel::load(const QString &fileName) {
    QFile file(fileName);
    bool success = false;
    if (file.open(QIODevice::ReadOnly)) {
        success = load(&file);
        file.close();
    }
    else success = false;

    return success;
}

bool JsonTableModel::load(QIODevice *device) {
    return loadJson(device->readAll());
}

bool JsonTableModel::loadJson(const QByteArray &json) {
    mDocument = QJsonDocument::fromJson(json);
    if (!mDocument.isNull())
    {
        beginResetModel();
        if (mDocument.isArray()) {
            root_item = JsonTreeItem::load(QJsonValue(mDocument.array()));
        } else {
            root_item = JsonTreeItem::load(QJsonValue(mDocument.object()));
        }
        endResetModel();
        return true;
    }
    return false;
}

QVariant JsonTableModel::data(const QModelIndex &index,
                              int role) const {
    if (!index.isValid())
        return QVariant();
    JsonTreeItem *item = static_cast<JsonTreeItem*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return QString("%1").arg(item->key());

        if (index.column() == 1)
            return QString("%1").arg(item->value());
    }
    return QVariant();
}

QVariant JsonTableModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {

        return mHeaders.value(section);
    } else {
        return QVariant();
    }
}

QModelIndex JsonTableModel::index(int row,
                                  int column,
                                  const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    JsonTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = root_item;
    else
        parentItem = static_cast<JsonTreeItem*>(parent.internalPointer());

    JsonTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex JsonTableModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    JsonTreeItem *childItem = static_cast<JsonTreeItem*>(index.internalPointer());
    JsonTreeItem *parentItem = childItem->parent();

    if (parentItem == root_item)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int JsonTableModel::rowCount(const QModelIndex &parent) const {
    JsonTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = root_item;
    else
        parentItem = static_cast<JsonTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int JsonTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 2;
}

bool JsonTableModel::setData(const QModelIndex& index,
                             const QVariant& value,
                             int role) {
    bool result = true;

    emit(dataChanged(index, index));
    return result;
}

Qt::ItemFlags JsonTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEnabled | Qt::ItemIsEditable |  Qt::ItemIsSelectable;
    return flags;
}


bool JsonTableModel::insertNewAttribute(const QModelIndex & parent,
                                        const QString& key,
                                        const  QJsonValue::Type json_type) {
    JsonTreeItem *parentItem =  NULL;

    if(parent.isValid()) {
        parentItem = static_cast<JsonTreeItem*>(parent.internalPointer());
        int row = parentItem->childCount();
        beginInsertRows(parent, row, row);
    } else {
        parentItem = root_item;
            beginResetModel();
    }

    //new attribute
    JsonTreeItem * child = new JsonTreeItem(parentItem);
    child->setKey(key);
    child->setType(json_type);
    parentItem->appendChild(child);
    if(parent.isValid()) {
        endInsertRows();
    } else {
        endResetModel();
    }
    return true;
}
