#include "SwitchAggregator.h"

#include <QDebug>

SwitchAggregator::SwitchAggregator(QObject *parent):
    QObject(parent) {

}

SwitchAggregator::~SwitchAggregator() {

}

void SwitchAggregator::addNewLogicSwitch(const QString& switch_name) {
    if(map_switch.count(switch_name)) return;
    MultiPropertyLogicSwitch *logic_switch_pointer = NULL;
    map_switch.insert(switch_name, QSharedPointer<MultiPropertyLogicSwitch>(logic_switch_pointer = new MultiPropertyLogicSwitch(switch_name)));
    connect(logic_switch_pointer,
            SIGNAL(switchChangeState(QString,bool)),
            SLOT(switchChangedState(QString,bool)));
}

void SwitchAggregator::addKeyRefValue(const QString& switch_name,
                                      const QString& key,
                                      const QString& reference_value) {
    if(map_switch.count(switch_name) == 0) return;
    map_switch[switch_name]->setPropertyKeyValue(key, reference_value);
}

void SwitchAggregator::broadcastCurrentValueForKey(const QString& key,
                                                   const QString& current_value) {
    qDebug() << "Brodcast  key: "<< key << " value:" << current_value;
    QMapIterator<QString, QSharedPointer<MultiPropertyLogicSwitch> > it(map_switch);
    while (it.hasNext()) {
        it.next();
        it.value()->currentPropertyKeyValue(key, current_value);
    }
}

void SwitchAggregator::connectSwitch(const QString& switch_name_where_attach,
                                     const QString& switch_name_to_attach,
                                     bool negation) {
    if(map_switch.count(switch_name_where_attach) == 0 ||
            map_switch.count(switch_name_to_attach) == 0 ) return;
    map_switch[switch_name_where_attach]->attachLogicSwitch(*map_switch[switch_name_to_attach], negation);
}

void    SwitchAggregator::switchChangedState(const QString& logic_switch_name,
                                             bool state) {
    qDebug() << "switchChangedState: "<< logic_switch_name << " state:" << state;
    QList< QSharedPointer<AbstractAttributeSetter> > setter_list = map_attribute_setter.values(logic_switch_name);
    foreach (QSharedPointer<AbstractAttributeSetter> setter, setter_list) {
        setter->setValueForState(state);
    }
}
