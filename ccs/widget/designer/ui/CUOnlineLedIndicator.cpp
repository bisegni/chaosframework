#include "CUOnlineLedIndicator.h"
#include <QDebug>
#include <QHBoxLayout>
#include "../../StateImageIndicatorWidget.h"
#include <QPainter>
CUOnlineLedIndicator::CUOnlineLedIndicator(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent),
    blink_counter(0),
    current_state(0),
    no_ts(new QIcon(":/images/white_circle_indicator.png")),
    timeouted(new QIcon(":/images/red_circle_indicator.png")),
    alive(new QIcon(":/images/green_circle_indicator.png")) {
    addState(0, no_ts);
    addState(1, timeouted);
    addState(2, alive);
    setState(0);
}

CUOnlineLedIndicator::~CUOnlineLedIndicator() {}

QSize CUOnlineLedIndicator::sizeHint() const
{
    return QSize(16, 16);
}

QSize CUOnlineLedIndicator::minimumSizeHint() const
{
    return QSize(16, 16);
}

void CUOnlineLedIndicator::addState(int state,
                                  QSharedPointer<QIcon> state_icon,
                                  bool blonk_on_repeat_set) {
    QMutexLocker l(&map_mutex);
    if(map_state_info.contains(state)) return;
    //add to internal map
    map_state_info.insert(state, QSharedPointer<CUOnlineLedIndicatorStateInfo>(new CUOnlineLedIndicatorStateInfo(state_icon, blonk_on_repeat_set)));
}


void CUOnlineLedIndicator::setState(int new_sate) {
    QMutexLocker l(&map_mutex);
//    if(new_sate==current_state) {
//        if(map_state_info[current_state]->blink_on_repeat_set) blink();
//        return;
//    }
    current_state = new_sate;
    repaint();
}

void CUOnlineLedIndicator::paintEvent(QPaintEvent *event) {
    qDebug() << QString("paintEvent on %1").arg(deviceID());
    QSize wSize = size();
    qDebug() << QString("size on %1 %2").arg(wSize.width(), wSize.height());
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

void CUOnlineLedIndicator::updateOnline(ChaosBaseDatasetUI::OnlineState state) {
    bool v = isEnabled();
    qDebug() << QString("updateOnline on %1").arg(deviceID());
    switch(state) {
    case OnlineStateNotFound:
    case OnlineStateUnknown:
        setState(0);
        break;
    case OnlineStateOFF:
        setState(1);
        break;
    case OnlineStateON:
        setState(2);
        break;
    }
}
void CUOnlineLedIndicator::updateValue(QVariant /*new_value*/) {}
