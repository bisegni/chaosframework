#include "AttributeValueChangeSet.h"

AttributeValueChangeSet::AttributeValueChangeSet(QObject *parent):
    QObject(parent),
    is_mandatory(false),
    parametrize(false),
    attribute_name(),
    type(0),
    last_value(),
    current_value(){}

AttributeValueChangeSet::~AttributeValueChangeSet() {

}

void AttributeValueChangeSet::setCurrentValue(const QVariant& _current_value) {
    current_value = _current_value;
}

void AttributeValueChangeSet::reset() {
    current_value = last_value;
}

void AttributeValueChangeSet::commit() {
    last_value = current_value;
}

