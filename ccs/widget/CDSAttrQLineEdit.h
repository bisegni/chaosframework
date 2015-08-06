#ifndef CDSAttrQLineEdit_H
#define CDSAttrQLineEdit_H

#include "../data/ChaosTypedAttributeValueSetter.h"

#include <QLineEdit>

class CDSAttrQLineEdit :
public QLineEdit {
    Q_OBJECT

    QSharedPointer<ChaosTypedAttributeValueSetter> chaos_attribute_value_setter_;
    Q_PROPERTY(QSharedPointer<ChaosTypedAttributeValueSetter> chaos_attribute_value_setter READ chaosAttributeValueSetter WRITE setChaosAttributeValueSetter)

public:
    CDSAttrQLineEdit(QWidget *obj = 0);
    CDSAttrQLineEdit(QSharedPointer<AttributeReader> attribute_reader,
                     QWidget *obj = 0);
    ~CDSAttrQLineEdit();

    QSharedPointer<ChaosTypedAttributeValueSetter> chaosAttributeValueSetter() const;
    void setChaosAttributeValueSetter(const QSharedPointer<ChaosTypedAttributeValueSetter>& chaos_attribute_value_setter);
private slots:
    void chaosValueTypeChanged(int new_type);
    void qlineEditTextChange();
};

#endif // CDSAttrQLineEdit_H
