#include "LedIndicatorWidget.h"

#include <QPainter>
LedIndicatorWidget::LedIndicatorWidget(QWidget *parent):
    QWidget(parent),
    current_state(0) {
}

void LedIndicatorWidget::paintEvent (QPaintEvent *)
{
    QPainter painter(this);
    if(map_state_icon.contains(current_state)) {
        QSize aSize = map_state_icon[current_state]->actualSize(size());
        painter.drawPixmap(QRect(0,0,width(),height()),
                           map_state_icon[current_state]->pixmap(aSize),
                           QRect(0,0,aSize.width(),aSize.height()));
    }else{
        //no state
    }

}

void LedIndicatorWidget::addState(int state_value,
                                  QSharedPointer<QIcon> state_icon) {
    if(map_state_icon.contains(state_value)) return;
    map_state_icon.insert(state_value, state_icon);
}


void LedIndicatorWidget::setState(int new_sate) {
    if(new_sate==current_state) return;
    current_state = new_sate;
    repaint();
}
