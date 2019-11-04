#include "CUOnlineLedndicator.h"
#include <QDebug>
#include <QHBoxLayout>
#include "../../StateImageIndicatorWidget.h"

CDatasetAttributeLedIndicator::CDatasetAttributeLedIndicator(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent),
    no_ts(new QIcon(":/images/white_circle_indicator.png")),
    timeouted(new QIcon(":/images/red_circle_indicator.png")),
    alive(new QIcon(":/images/green_circle_indicator.png")) {
    StateImageIndicatorWidget *indicator_widget = new StateImageIndicatorWidget(this);
    indicator_widget->setObjectName("indicator");
    indicator_widget->addState(0, no_ts);
    indicator_widget->addState(1, timeouted);
    indicator_widget->addState(2, alive);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(indicator_widget);
    setLayout(layout);

}

CDatasetAttributeLedIndicator::~CDatasetAttributeLedIndicator() {}

void CDatasetAttributeLedIndicator::updateOnlineState(OnlineState state) {
    StateImageIndicatorWidget *indicator = findChild<StateImageIndicatorWidget *>("indicator");
    if(indicator == nullptr) return;
    switch(state) {
    case OnlineStateNotFound:
    case OnlineStateUnknown:
        indicator->setState(0);
        break;
    case OnlineStateOFF:
        indicator->setState(1);
        break;
    case OnlineStateON:
        indicator->setState(2);
        break;
    }
}

void CDatasetAttributeLedIndicator::updateValue(QVariant /*new_value*/) {}
