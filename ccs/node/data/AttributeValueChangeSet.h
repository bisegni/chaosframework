#ifndef ATTRIBUTEVALUECHANGESET_H
#define ATTRIBUTEVALUECHANGESET_H
#include <QObject>
#include <QVariant>
#include <QSharedPointer>
#include <chaos/common/data/CDataWrapper.h>
class AttributeValueChangeSet:
public QObject {
    Q_OBJECT
public:
    bool is_mandatory;
    bool parametrize;
    QString  attribute_name;
    int type;
    QVariant last_value;
    QVariant current_value;
    QSharedPointer<chaos::common::data::CDataWrapper> attribute_raw_description;
    explicit AttributeValueChangeSet(QObject *parent = 0);
    ~AttributeValueChangeSet();
    void setCurrentValue(const QVariant& cur_val);
    void reset();
    void commit();
};

#endif // ATTRIBUTEVALUECHANGESET_H
