#include "CDSAttrQLineEdit.h"
#include <QRegExpValidator>
#include <QIntValidator>
#include <QDoubleValidator>
#include <climits>
#include <QDebug>

CDSAttrQLineEdit::CDSAttrQLineEdit(QWidget *obj) :
    QLineEdit(obj) {

}

CDSAttrQLineEdit::CDSAttrQLineEdit(QSharedPointer<AttributeReader> attribute_reader,
                                   QWidget *obj):
    QLineEdit(obj) {
    connect(this,
            SIGNAL(editingFinished()),
            SLOT(qlineEditTextChange()));
    setChaosAttributeValueSetter(QSharedPointer<ChaosTypedAttributeValueSetter>(new ChaosTypedAttributeValueSetter()));
    //connect to the change of type
    connect(chaosAttributeValueSetter().data(),
            SIGNAL(valueTypeChange(int)),
            SLOT(chaosValueTypeChanged(int)));
    //setup with reader
    chaosAttributeValueSetter()->setupWithAttributeReader(attribute_reader);
}

CDSAttrQLineEdit::~CDSAttrQLineEdit() {

}

QSharedPointer<ChaosTypedAttributeValueSetter> CDSAttrQLineEdit::chaosAttributeValueSetter() const {
    return chaos_attribute_value_setter_;
}

void CDSAttrQLineEdit::setChaosAttributeValueSetter(const QSharedPointer<ChaosTypedAttributeValueSetter>& chaos_attribute_value_setter) {
    chaos_attribute_value_setter_ = chaos_attribute_value_setter;
}

void CDSAttrQLineEdit::chaosValueTypeChanged(int new_type) {
    //setup the input patter according to the type
    switch(new_type) {
    case chaos::DataType::TYPE_BOOLEAN: {
        QRegExpValidator *validator = new QRegExpValidator(QRegExp(tr("[01]{1}")), this);
        setValidator(validator);
        break;
    }
    case chaos::DataType::TYPE_INT32: {
        QIntValidator *validator = new QIntValidator(std::numeric_limits<int32_t>::min(),
                                                     std::numeric_limits<int32_t>::max(),
                                                     this);
        setValidator(validator);
        break;
    }
    case chaos::DataType::TYPE_INT64:{
        QIntValidator *validator = new QIntValidator(std::numeric_limits<int64_t>::min(),
                                                     std::numeric_limits<int64_t>::max(),
                                                     this);
        setValidator(validator);
        break;
    }
    case chaos::DataType::TYPE_DOUBLE:{
        QDoubleValidator *validator = new QDoubleValidator(-1 * std::numeric_limits<double>::max(),
                                                           std::numeric_limits<double>::max(),
                                                           2,
                                                           this);
        validator->setNotation(QDoubleValidator::StandardNotation);
        setValidator(validator);
        break;
    }
    case chaos::DataType::TYPE_STRING:
        break;
    default:
        break;
    }
}

void CDSAttrQLineEdit::qlineEditTextChange() {
    chaosAttributeValueSetter()->setChaosAttributeValue(text());
}
