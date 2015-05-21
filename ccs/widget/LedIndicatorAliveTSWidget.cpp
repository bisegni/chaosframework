#include "LedIndicatorAliveTSWidget.h"

#include <QDateTime>

LedIndicatorAliveTSWidget::LedIndicatorAliveTSWidget(QWidget *parent):
    LedIndicatorWidget(parent),
    no_ts(new QIcon(":/images/white_circle_indicator.png")),
    timeouted(new QIcon(":/images/red_circle_indicator.png")),
    alive(new QIcon(":/images/green_circle_indicator.png")),
    last_recevied_ts(0) {
    addState(0, no_ts);
    addState(1, timeouted);
    addState(2, alive);
}

void LedIndicatorAliveTSWidget::setNewTS(uint64_t current_timestamp) {
    if(current_timestamp == 0) {
        setState(0);
    } else {
        uint64_t time_diff = QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch() - current_timestamp;
        if(time_diff <= 5000) {
            //in time
            setState(2);
        } else {
            //timeouted
            setState(1);
        }
    }
}
