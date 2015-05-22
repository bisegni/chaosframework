#include "MultiPropertyLogicSwitch.h"

#include <QDebug>

MultiPropertyLogicSwitch::MultiPropertyLogicSwitch(const QString& _logic_switch_name,
                                                   QObject *parent) :
    QObject(parent),
    logic_switch_name(_logic_switch_name),
    logic_state_bit_field(){

}

void MultiPropertyLogicSwitch::setPropertyKeyValue(const QString& key,
                                                   const QString& value) {
    QMutexLocker l(&mutex);
    QSharedPointer<KeyValueAndPosition> kvp(new KeyValueAndPosition());
    kvp->reference_value = value;

    logic_state_bit_field.push_back(false);
    kvp->bit_field_position = logic_state_bit_field.size()-1;
    qDebug() << "MultiPropertyLogicSwitch::setPropertyKeyValue->key:" << key << " Ref Value:"<<value << " bit pos:" << (logic_state_bit_field.size()-1);

    property_map.insert(key, kvp);
}

//! set the key value
void MultiPropertyLogicSwitch::currentPropertyKeyValue(const QString& key,
                                                       const QString& value) {
    QMutexLocker l(&mutex);
    if(property_map.count(key) == 0) return;
    QSharedPointer<KeyValueAndPosition>& kvp = property_map[key];
    //update key bit
    qDebug() << "MultiPropertyLogicSwitch::currentPropertyKeyValue->key:" << key << " Ref Value:"<<kvp->reference_value <<" value: " << value;
    logic_state_bit_field[kvp->bit_field_position] = (kvp->reference_value.compare(value)==0);
    emit switchChangeState(logic_switch_name, logic_state_bit_field.all());
}

void MultiPropertyLogicSwitch::attachLogicSwitch(const MultiPropertyLogicSwitch& logic_switch,
                                                 bool negation = false) {
    setPropertyKeyValue(logic_switch.logic_switch_name, (negation?"OFF":"ON"));
    connect(&logic_switch,
            SIGNAL(switchChangeState(QString,bool)),
            SLOT(connectedSwitchChangedState(QString,bool)));
}

void MultiPropertyLogicSwitch::connectedSwitchChangedState(const QString& logic_switch_name,
                                                           bool state) {
    currentPropertyKeyValue(logic_switch_name, (state?"ON":"OFF"));
}
