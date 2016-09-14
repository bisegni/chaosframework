#ifndef CSTATEVISIBLEPUSHBUTTON_H
#define CSTATEVISIBLEPUSHBUTTON_H

#include <QPushButton>

class CStateVisiblePushButton :
        public QPushButton {
public:
    CStateVisiblePushButton(QWidget *parent = 0);
    void setButtonState(bool new_state);
protected:
    void paintEvent(QPaintEvent *p_event);
private:
    bool state_on;
};

#endif // CSTATEVISIBLEPUSHBUTTON_H
