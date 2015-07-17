#include "MultiPropertyLogicSwitch.h"

#include <QDebug>

#include<boost/version.hpp>

MultiPropertyLogicSwitch::MultiPropertyLogicSwitch(const QString& _logic_switch_name,
                                                   QObject *parent) :
    QObject(parent),
    last_output_state(-1),
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
    if(last_output_state == -1) {
#if (BOOST_VERSION /100000) >= 1 && ((BOOST_VERSION /100) % 1000) >= 56
        last_output_state = logic_state_bit_field.all();
#else
        last_output_state = logic_state_bit_field.count() == property_map.size();
#endif
    } else {
#if (BOOST_VERSION /100000) >= 1 && ((BOOST_VERSION /100) % 1000) >= 56
        if(((bool)last_output_state) == logic_state_bit_field.all()) return;
        last_output_state = logic_state_bit_field.all();
#else
        if(((bool)last_output_state) == (logic_state_bit_field.count() == property_map.size())) return;
        last_output_state = (logic_state_bit_field.count() == property_map.size());
#endif

    }
    emit switchChangeState(logic_switch_name, last_output_state);
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
