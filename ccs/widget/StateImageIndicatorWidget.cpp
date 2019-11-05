#include "StateImageIndicatorWidget.h"

#include <QPainter>
StateImageIndicatorWidget::StateImageIndicatorWidget(QWidget *parent):
    QWidget(parent),
    blink_counter(0),
    current_state(0),
    map_mutex(QMutex::Recursive){
    connect(&blink_timer,
            SIGNAL(timeout()),
            SLOT(tick()));
    blink_timer.setInterval(250);
}

void StateImageIndicatorWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QMutexLocker l(&map_mutex);
    if(map_state_info.contains(current_state) &&
            (blink_counter%2) == 0) {
        if(map_state_info[current_state]->icon.isNull() == false) {
            QSize aSize = map_state_info[current_state]->icon->actualSize(size());
            painter.drawPixmap(QRect(0,0,width(),height()),
                               map_state_info[current_state]->icon->pixmap(aSize),
                               QRect(0,0,aSize.width(),aSize.height()));
        }
    } else {
        //no state
    }

}

void StateImageIndicatorWidget::addState(int state,
                                  QSharedPointer<QIcon> state_icon,
                                  bool blonk_on_repeat_set) {
    QMutexLocker l(&map_mutex);
    if(map_state_info.contains(state)) return;
    //add to internal map
    map_state_info.insert(state, QSharedPointer<StateInfo>(new StateInfo(state_icon, blonk_on_repeat_set)));
}


void StateImageIndicatorWidget::setState(int new_sate) {
    QMutexLocker l(&map_mutex);
    if(new_sate==current_state) {
        if(map_state_info[current_state]->blink_on_repeat_set) blink();
        return;
    }
    current_state = new_sate;
    repaint();
}

void StateImageIndicatorWidget::setStateBlinkOnRepeatSet(int state,
                                                  bool blonk_on_repeat_set) {
    QMutexLocker l(&map_mutex);
    if(!map_state_info.contains(state)) return;
    map_state_info[state]->blink_on_repeat_set = blonk_on_repeat_set;
}

void StateImageIndicatorWidget::blink() {
    blink_timer.start();
}

int StateImageIndicatorWidget::getState() {
    return current_state;
}

void StateImageIndicatorWidget::tick() {
    if((++blink_counter%2) == 0){
        blink_timer.stop();
    }
    repaint();
}
