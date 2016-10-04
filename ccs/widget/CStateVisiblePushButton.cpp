#include "CStateVisiblePushButton.h"

#include <QPen>
#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionButton>
#define BORDER_OFFSET 4

CStateVisiblePushButton::StateInfo::StateInfo():
    state_description("Default Tooltip"),
    state_color(Qt::lightGray){}

CStateVisiblePushButton::StateInfo::StateInfo(const QString& _state_description,
                                              const QColor& _state_color):
    state_description(_state_description),
    state_color(_state_color){}

CStateVisiblePushButton::StateInfo::StateInfo(const CStateVisiblePushButton::StateInfo& _state_info):
    state_description(_state_info.state_description),
    state_color(_state_info.state_color){}


CStateVisiblePushButton::CStateVisiblePushButton(QWidget *parent):
    current_state(0){
    setStyleSheet("text-align:bottom center;");
}

unsigned int CStateVisiblePushButton::addState(const CStateVisiblePushButton::StateInfo& new_state) {
    map_lock.lockForWrite();
    unsigned int new_id = map_state_info.size();
    map_state_info.insert(new_id, new_state);
    map_lock.unlock();
    return new_id;
}

void CStateVisiblePushButton::updateStateDescription(unsigned int state,
                                                     const QString& state_descitpion) {
    if(map_state_info.contains(state) == false) return;
    map_lock.lockForRead();
    map_state_info[state].state_description = state_descitpion;
    map_lock.unlock();
    repaint();
}

void CStateVisiblePushButton::updateStateColor(unsigned int state,
                                               const QColor& state_color) {
    if(map_state_info.contains(state) == false) return;
    map_lock.lockForRead();
    map_state_info[state].state_color = state_color;
    map_lock.unlock();
    repaint();
}

void CStateVisiblePushButton::setButtonState(unsigned int new_state) {

    if(map_state_info.contains(new_state) == false) return;
    current_state = new_state;
    map_lock.lockForRead();
    setToolTip(map_state_info[current_state].state_description);
    map_lock.unlock();
    repaint();
}

unsigned int CStateVisiblePushButton::currentState() {
 return current_state;
}

void CStateVisiblePushButton::paintEvent(QPaintEvent *p_event) {
    //QPushButton::paintEvent(p_event);
    qreal render_width = ((qreal)width()-(BORDER_OFFSET*2));
    //draw rectanle led indicator for represent the state
    QPainter painter(this);

    //render push button background
    QStylePainter stype_painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    //disable drawing text with style option
    option.text = "";
    stype_painter.drawControl(QStyle::CE_PushButton, option);

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRect(3, 3, render_width, 3);

    QPen border_pen(Qt::gray, 1);
    painter.setPen(border_pen);
    //lock map
    map_lock.lockForRead();
    painter.fillPath(path, map_state_info[current_state].state_color);
    map_lock.unlock();
    painter.drawPath(path);
    painter.setPen(QPen(isEnabled()?QApplication::palette().Inactive:QApplication::palette().Active));
    QFont font = painter.font();
    font.setPointSize(9);
    font.setWeight(QFont::Bold);
    painter.setFont(font);

    QFontMetrics fm(font);
    const QString printable_string = fm.elidedText(text(), Qt::ElideRight,  render_width);
    qreal text_start_x = (((qreal)width()/(qreal)2) - ((qreal)fm.width(printable_string)/(qreal)2));
    painter.drawText(QPointF(text_start_x, height()-3), printable_string);
}
