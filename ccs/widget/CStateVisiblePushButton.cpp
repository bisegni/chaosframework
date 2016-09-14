#include "CStateVisiblePushButton.h"

#include <QPen>
#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionButton>
#define BORDER_OFFSET 3

CStateVisiblePushButton::CStateVisiblePushButton(QWidget *parent):
state_on(false){
    setStyleSheet("text-align:bottom center;");
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
    path.addRect(3, 3, render_width, 2);

    QPen border_pen(Qt::gray, 1);
    painter.setPen(border_pen);
    painter.fillPath(path, state_on?Qt::red:Qt::lightGray);
    painter.drawPath(path);
    painter.setPen(QPen(isEnabled()?QApplication::palette().buttonText().color():Qt::lightGray));
    QFont font = painter.font();
    font.setPointSize(8);
    font.setWeight(QFont::Bold);
    painter.setFont(font);

    QFontMetrics fm(font);
    const QString printable_string = fm.elidedText(text(), Qt::ElideRight,  render_width);
    qreal text_start_x = (((qreal)width()/(qreal)2) - ((qreal)fm.width(printable_string)/(qreal)2));
    painter.drawText(QPointF(text_start_x, height()-3), printable_string);
}

void CStateVisiblePushButton::setButtonState(bool new_state) {
    state_on = new_state;
    repaint();
}
