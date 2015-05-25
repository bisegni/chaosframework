#ifndef MULTIPROPERTYLOGICSWITCH_H
#define MULTIPROPERTYLOGICSWITCH_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QMutex>
#include <QMutexLocker>
#include <QSharedPointer>

#include <boost/atomic.hpp>
#include <boost/dynamic_bitset.hpp>

struct KeyValueAndPosition {
    QSet<QString> reference_value;
    int bit_field_position;
};

class MultiPropertyLogicSwitch :
        public QObject {
    Q_OBJECT
public:
    explicit MultiPropertyLogicSwitch(const QString& _logic_switch_name,
                                      QObject *parent = 0);

    //!set the value for the key that help to trigger the switch state on
    void setPropertyKeyValue(const QString& key,
                             const QString& value);

    //! set the key value
    void currentPropertyKeyValue(const QString& key,
                                 const QString& value);

    void attachLogicSwitch(const MultiPropertyLogicSwitch& logic_switch,
                           bool negation);
signals:
    void switchChangeState(const QString& logic_switch_name,
                           bool state);
private slots:
    void connectedSwitchChangedState(const QString& logic_switch_name,
                                     bool state);
private:
    QMutex mutex;
    QMap<QString, QSharedPointer<KeyValueAndPosition> > property_map;
    const QString logic_switch_name;
    //! it determinate ifthe switch is on or false
    boost::dynamic_bitset<> logic_state_bit_field;
};

#endif // MULTIPROPERTYLOGICSWITCH_H
