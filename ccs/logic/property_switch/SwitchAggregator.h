#ifndef SWITCHAGGREGATOR_H
#define SWITCHAGGREGATOR_H

#include "MultiPropertyLogicSwitch.h"

#include <QObject>
#include <QMap>
#include <QMultiMap>
#include <QSharedPointer>

class AbstractAttributeSetter {
public:
    virtual void setValueForState(bool switch_state) = 0;
};

template<typename T>
class TemplatedAbstractAttributeSetter:
        public AbstractAttributeSetter {
    QObject *target;
    const QString property_name;
    T value_on;
    T value_off;

public:

    TemplatedAbstractAttributeSetter(QObject *_target,
                                     const QString& _property_name,
                                     T _value_on,
                                     T _value_off):
        target(_target),
        property_name(_property_name),
        value_on(_value_on),
        value_off(_value_off){}
    void setValueForState(bool switch_state) {
        target->setProperty(property_name.toStdString().c_str(), (switch_state?value_on:value_off));
    }
};

class SwitchAggregator:
        public QObject {
    Q_OBJECT
public:
    explicit SwitchAggregator(QObject *parent = 0);

    ~SwitchAggregator();

    void addNewLogicSwitch(const QString& switch_name);

    void addKeyRefValue(const QString& switch_name,
                        const QString& key,
                        const QString& reference_value);

    void broadcastCurrentValueForKey(const QString& key,
                                     const QString& current_value);

    void connectSwitch(const QString& switch_name_where_attach,
                       const QString& switch_name_to_attach,
                       bool negation = false);

    template<typename T>
    void attachObjectAttributeToSwitch(const QString& switch_name,
                                       QObject *obj_target,
                                       const QString& property_name,
                                       T value_on,
                                       T value_off) {
        map_attribute_setter.insert(switch_name, QSharedPointer<AbstractAttributeSetter>(new TemplatedAbstractAttributeSetter<T>(obj_target,
                                                                                                                                 property_name,
                                                                                                                                 value_on,
                                                                                                                                 value_off)));
    }
signals:
    void stateChangedOnSwitch(const QString& logic_switch_name,
                              bool state);
private slots:
    void switchChangedState(const QString& logic_switch_name,
                            bool state);
private:
    QMutex mutex;
    QMap<QString, QSharedPointer<MultiPropertyLogicSwitch> > map_switch;
    QMultiMap<QString, QSharedPointer<AbstractAttributeSetter> > map_attribute_setter;
};
#endif // SWITCHAGGREGATOR_H
