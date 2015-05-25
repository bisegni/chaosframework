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
    QSharedPointer<KeyValueAndPosition> kvp;
    if(property_map.count(key) == 0) {
        // create a new kvp
        kvp = QSharedPointer<KeyValueAndPosition>(new KeyValueAndPosition());
        //add it to bitfield
        logic_state_bit_field.push_back(false);
        //register the kvp position in bitfield
        kvp->bit_field_position = logic_state_bit_field.size()-1;
        //insert new key in map
        property_map.insert(key, kvp);
    } else {
        //get altready inserted key
        kvp = property_map[key];
    }
    //add new value
    kvp->reference_value.insert(value);
}

//! set the key value
void MultiPropertyLogicSwitch::currentPropertyKeyValue(const QString& key,
                                                       const QString& value) {
    QMutexLocker l(&mutex);
    if(property_map.count(key) == 0) return;
    QSharedPointer<KeyValueAndPosition>& kvp = property_map[key];
    //update key bit
    logic_state_bit_field[kvp->bit_field_position] = (kvp->reference_value.find(value)!=kvp->reference_value.end());
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
