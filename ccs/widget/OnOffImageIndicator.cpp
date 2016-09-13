#include "OnOffImageIndicator.h"

OnOffImageIndicator::OnOffImageIndicator(QWidget *parent):
    StateImageIndicatorWidget(parent),
    off_state_icon(new QIcon(":/images/led_red.png")),
    on_state_icon(new QIcon(":/images/led_green.png")){
    addState(0, off_state_icon);
    addState(1, on_state_icon);
    setMinimumSize(QSize(10,10));
    setMaximumSize(QSize(10,10));
}

OnOffImageIndicator::~OnOffImageIndicator() {}

void OnOffImageIndicator::on() {
    setState(1);
}

void OnOffImageIndicator::off() {
    setState(0);
}

void OnOffImageIndicator::setState(bool on) {
     StateImageIndicatorWidget::setState(on?1:0);
}
