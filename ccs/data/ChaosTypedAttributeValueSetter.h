#ifndef ChaosTypedAttributeValueSetter_H
#define ChaosTypedAttributeValueSetter_H

#include "AttributeReader.h"

#include <QObject>
#include <QVariant>
#include <QSharedPointer>

class ChaosTypedAttributeValueSetter :
        public QObject {
    Q_OBJECT

    QString chaos_target_uid_;
    Q_PROPERTY(QString chaos_target_uid READ chaosTargetUID WRITE setChaosTargetUID)

    QString chaos_attribute_name_;
    Q_PROPERTY(QString chaos_attribute_name READ chaosAttributeName WRITE setChaosAttributeName)

    int chaos_type_;
    Q_PROPERTY(int chaos_type READ chaosType WRITE setChaosType)

    QVariant chaos_attribute_value_;
    Q_PROPERTY(QVariant chaos_attribute_value READ chaosAttributeValue WRITE setChaosAttributeValue)

    bool chaos_attribute_mandatory_flag_;
    Q_PROPERTY(bool chaos_attribute_mandatory READ chaosAttributeMandatoryFlag WRITE setChaosAttributeMandatoryFlag)
public:
    explicit ChaosTypedAttributeValueSetter(QObject *parent = 0);
            ChaosTypedAttributeValueSetter(const ChaosTypedAttributeValueSetter& source);
    explicit ChaosTypedAttributeValueSetter(QSharedPointer<AttributeReader> attribute_reader,
                                            QObject *parent = 0);
    ~ChaosTypedAttributeValueSetter();

    void setupWithAttributeReader(QSharedPointer<AttributeReader> attribute_reader);

    QString chaosAttributeName() const;
    void setChaosAttributeName(const QString& chaos_attribute_name);

    QString chaosTargetUID() const;
    void setChaosTargetUID(const QString& chaos_target_uid);

    int chaosType() const;
    void setChaosType(int chaos_type);

    QVariant chaosAttributeValue() const;
    void setChaosAttributeValue(QVariant chaos_attribute_value);

    bool chaosAttributeMandatoryFlag() const;
    void setChaosAttributeMandatoryFlag(bool chaos_attribute_mandatory);

    bool isValid();
    ChaosSharedPtr<chaos::common::data::CDataWrapperKeyValueSetter> getCDataWrapperValueSetter(bool *ok = NULL);

signals:
    void valueTypeChange(int type);

    void valueSet(const QString& chaos_target_uid,
                  const QString& chaos_attribute_name,
                  int chaos_type,
                  QVariant chaos_value);
    void valueChangedSet(const QString& chaos_target_uid,
                         const QString& chaos_attribute_name,
                         int chaos_type,
                         QVariant chaos_value);
public slots:
    void commitChaosValue();
    void rollbackChaosValue();
};

#endif // ChaosTypedAttributeValueSetter_H
