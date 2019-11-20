#include "CStatePushButton.h"

#include <QPen>
#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionButton>
#define BORDER_OFFSET 4

CStatePushButton::StateInfo::StateInfo():
    state_description("Default Tooltip"),
    state_color(Qt::lightGray){}

CStatePushButton::StateInfo::StateInfo(const QString& _state_description,
                                              const QColor& _state_color):
    state_description(_state_description),
    state_color(_state_color){}

CStatePushButton::StateInfo::StateInfo(const CStatePushButton::StateInfo& _state_info):
    state_description(_state_info.state_description),
    state_color(_state_info.state_color){}


CStatePushButton::CStatePushButton(QWidget *parent):
    QPushButton(parent),
    current_state(0){
    setStyleSheet("text-align:bottom center;");
}

unsigned int CStatePushButton::addState(const CStatePushButton::StateInfo& new_state) {
    map_lock.lockForWrite();
    unsigned int new_id = static_cast<unsigned int>(map_state_info.size());
    map_state_info.insert(new_id, new_state);
    map_lock.unlock();
    return new_id;
}

void CStatePushButton::updateStateDescription(unsigned int state,
                                                     const QString& state_descitpion) {
    if(map_state_info.contains(state) == false) return;
    map_lock.lockForRead();
    map_state_info[state].state_description = state_descitpion;
    map_lock.unlock();
    repaint();
}

void CStatePushButton::updateStateColor(unsigned int state,
                                               const QColor& state_color) {
    if(map_state_info.contains(state) == false) return;
    map_lock.lockForRead();
    map_state_info[state].state_color = state_color;
    map_lock.unlock();
    repaint();
}

void CStatePushButton::setButtonState(unsigned int new_state) {

    if(map_state_info.contains(new_state) == false) return;
    current_state = new_state;
    map_lock.lockForRead();
    setToolTip(map_state_info[current_state].state_description);
    map_lock.unlock();
    repaint();
}

unsigned int CStatePushButton::currentState() {
 return current_state;
}

void CStatePushButton::paintEvent(QPaintEvent *p_event) {
    Q_UNUSED(p_event)
    const int distance = 10;
    QPushButton::paintEvent(p_event);
    QPainter painter(this);

    //render push button background
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect indicator_rect = QRect(distance, 6, width() - 2 * distance, 3);
    QPen border_pen(Qt::gray, 1);
    painter.setPen(border_pen);
    //lock map
    map_lock.lockForRead();
    painter.fillRect(indicator_rect, map_state_info[current_state].state_color);
    map_lock.unlock();
    painter.drawRect(indicator_rect);
    painter.setPen(QPen(isEnabled()?QApplication::palette().Inactive:QApplication::palette().Active));

}
