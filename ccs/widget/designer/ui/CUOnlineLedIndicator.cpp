#include "CUOnlineLedIndicator.h"
#include <QDebug>
#include <QHBoxLayout>
#include "../../StateImageIndicatorWidget.h"
#include <QPainter>
#include <QTime>
#include <QFile>

CUOnlineLedIndicator::CUOnlineLedIndicator(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent),
    current_state(0),
    map_mutex(QMutex::Recursive){
    timeouted.reset(new QIcon(":/images/red_circle_indicator.png"));
    alive.reset(new QIcon(":/images/green_circle_indicator.png"));
    if(QFile::exists(":/images/green_circle_indicator.png"))
    addState(0, timeouted);
    addState(1, alive);
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
    current_state = new_sate;
    repaint();
}

void CUOnlineLedIndicator::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QMutexLocker l(&map_mutex);
    if(map_state_info.contains(current_state)) {
        if(map_state_info[current_state]->icon.isNull() == false) {
            map_state_info[current_state]->icon->paint(&painter, rect());
        }
    }

    ChaosBaseDatasetAttributeUI::paintEvent(event);
}

void CUOnlineLedIndicator::updateOnline(ChaosBaseDatasetUI::OnlineState state) {
    qDebug() << QString("updateOnline on %1 with state %2").arg(deviceID()).arg(state);
    switch(state) {
    case OnlineStateNotFound:
    case OnlineStateUnknown:
    case OnlineStateOFF:
        setState(0);
        break;
    case OnlineStateON:
        setState(1);
        break;
    }
}
void CUOnlineLedIndicator::updateValue(QVariant variant_value) {}
