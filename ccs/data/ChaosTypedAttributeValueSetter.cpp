#include "ChaosTypedAttributeValueSetter.h"

using namespace chaos::common::data;

ChaosTypedAttributeValueSetter::ChaosTypedAttributeValueSetter(QObject *parent) :
    QObject(parent) {

}

ChaosTypedAttributeValueSetter::ChaosTypedAttributeValueSetter(const ChaosTypedAttributeValueSetter& source) {
    setChaosTargetUID(source.chaosTargetUID());
    setChaosAttributeName(source.chaosAttributeName());
    setChaosType(source.chaosType());
    setChaosAttributeValue(source.chaosAttributeValue());
}

ChaosTypedAttributeValueSetter::ChaosTypedAttributeValueSetter(QSharedPointer<AttributeReader> attribute_reader,
                                                               QObject *parent):
    QObject(parent) {
    setupWithAttributeReader(attribute_reader);
}

ChaosTypedAttributeValueSetter::~ChaosTypedAttributeValueSetter() {

}

void ChaosTypedAttributeValueSetter::setupWithAttributeReader(QSharedPointer<AttributeReader> attribute_reader) {
    setChaosAttributeName(attribute_reader->getName());
    setChaosType(attribute_reader->getType());
}

QString ChaosTypedAttributeValueSetter::chaosAttributeName() const {
    return chaos_attribute_name_;
}

void ChaosTypedAttributeValueSetter::setChaosTargetUID(const QString& chaos_target_uid) {
    chaos_target_uid_ = chaos_target_uid;
}

int ChaosTypedAttributeValueSetter::chaosType() const {
    return chaos_type_;
}

void ChaosTypedAttributeValueSetter::setChaosAttributeName(const QString& chaos_attribute_name) {
    chaos_attribute_name_ = chaos_attribute_name;
}

QString ChaosTypedAttributeValueSetter::chaosTargetUID() const {
    return chaos_target_uid_;
}

void ChaosTypedAttributeValueSetter::setChaosType(int chaos_type) {
    if(chaos_type_ == chaos_type) return;
    chaos_type_ = chaos_type;
    emit valueTypeChange(chaos_type_);
}

QVariant ChaosTypedAttributeValueSetter::chaosAttributeValue() const{
    return chaos_attribute_value_;
}

void ChaosTypedAttributeValueSetter::setChaosAttributeValue(QVariant chaos_attribute_value) {
    chaos_attribute_value_ = chaos_attribute_value;
}

QSharedPointer<CDataWrapperKeyValueSetter> ChaosTypedAttributeValueSetter::getCDataWrapperValueSetter(bool *ok) {
    if(chaos_attribute_value_.isNull()) return QSharedPointer<CDataWrapperKeyValueSetter>();

    switch(chaosType()) {
    case chaos::DataType::TYPE_BOOLEAN:
        if(ok) *ok = true;
        return QSharedPointer<CDataWrapperKeyValueSetter>(new CDataWrapperBoolKeyValueSetter(chaosAttributeName().toStdString(), chaos_attribute_value_.toBool()));
        break;
    case chaos::DataType::TYPE_INT32:
        return QSharedPointer<CDataWrapperKeyValueSetter>(new CDataWrapperBoolKeyValueSetter(chaosAttributeName().toStdString(), chaos_attribute_value_.toInt(ok)));
        break;
    case chaos::DataType::TYPE_INT64:
        return QSharedPointer<CDataWrapperKeyValueSetter>(new CDataWrapperBoolKeyValueSetter(chaosAttributeName().toStdString(), chaos_attribute_value_.toLongLong(ok)));
        break;
    case chaos::DataType::TYPE_DOUBLE:
        return QSharedPointer<CDataWrapperKeyValueSetter>(new CDataWrapperBoolKeyValueSetter(chaosAttributeName().toStdString(), chaos_attribute_value_.toDouble(ok)));
        break;
    case chaos::DataType::TYPE_STRING:
        if(ok) *ok = true;
        return QSharedPointer<CDataWrapperKeyValueSetter>(new CDataWrapperStringKeyValueSetter(chaosAttributeName().toStdString(), chaos_attribute_value_.toString().toStdString()));
        break;
    default:
        if(ok) *ok = false;
        return QSharedPointer<CDataWrapperKeyValueSetter> ();
        break;
    }
}

void ChaosTypedAttributeValueSetter::commitChaosValue() {

}

void ChaosTypedAttributeValueSetter::rollbackChaosValue() {

}

