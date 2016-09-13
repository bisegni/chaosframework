#ifndef ONOFFIMAGEINDICATOR_H
#define ONOFFIMAGEINDICATOR_H

#include "StateImageIndicatorWidget.h"

class OnOffImageIndicator:
public StateImageIndicatorWidget {
public:
    OnOffImageIndicator(QWidget *parent = 0);
    ~OnOffImageIndicator();

    void on();
    void off();
    void setState(bool on);
private:
    QSharedPointer<QIcon> off_state_icon;
    QSharedPointer<QIcon> on_state_icon;
};

#endif // ONOFFIMAGEINDICATOR_H
