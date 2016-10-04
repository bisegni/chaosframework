#ifndef CSTATEVISIBLEPUSHBUTTON_H
#define CSTATEVISIBLEPUSHBUTTON_H

#include <QMap>
#include <QPushButton>
#include <QReadWriteLock>

class CStateVisiblePushButton :
        public QPushButton {

public:
    struct StateInfo {
        QString state_description;
        QColor state_color;

        StateInfo();
        StateInfo(const QString& _state_description,
                  const QColor& _state_color);
        StateInfo(const StateInfo& _state_info);
    };

    CStateVisiblePushButton(QWidget *parent = 0);
    unsigned int currentState();
    void setButtonState(unsigned int new_state);
    unsigned int addState(const StateInfo& new_state);
    void updateStateDescription(unsigned int state,
                                const QString& state_descitpion);

    void updateStateColor(unsigned int state,
                          const QColor& state_color);
protected:
    void paintEvent(QPaintEvent *p_event);
private:
    unsigned int current_state;
    QReadWriteLock       map_lock;
    QMap<int, StateInfo> map_state_info;
};

#endif // CSTATEVISIBLEPUSHBUTTON_H
