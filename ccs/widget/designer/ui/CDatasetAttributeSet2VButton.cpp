#include "CDatasetAttributeSet2VButton.h"

#include <QDebug>
#include <QDateTime>
#include <QHBoxLayout>

CDatasetAttributeSet2VButton::CDatasetAttributeSet2VButton(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent),
    _internal_state(StateUndeterminated){
    cstate_push_button = new CStatePushButton(this);
    cstate_push_button->addState(CStatePushButton::StateInfo("Undeterminated", Qt::lightGray));
    cstate_push_button->addState(CStatePushButton::StateInfo("Off", Qt::darkGreen));
    cstate_push_button->addState(CStatePushButton::StateInfo("On", Qt::green));
//    cstate_push_button->setCheckable(true);
    connect(cstate_push_button,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(-1);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(cstate_push_button);
    setLayout(layout);
}

QString CDatasetAttributeSet2VButton::attributeSetValueOn() {
    return p_attribute_setValue_on;
}

void CDatasetAttributeSet2VButton::setAttributeSetValueOn(QString& new_p_attribute_setValue_on) {
    p_attribute_setValue_on = new_p_attribute_setValue_on;
}

QString CDatasetAttributeSet2VButton::attributeSetValueOff() {
    return p_attribute_setValue_off;
}

void CDatasetAttributeSet2VButton::setAttributeSetValueOff(QString& new_p_attribute_setValue_off) {
    p_attribute_setValue_off = new_p_attribute_setValue_off;
}

QSize CDatasetAttributeSet2VButton::sizeHint() const {
    return QSize(64, 32);
}

QSize CDatasetAttributeSet2VButton::minimumSizeHint() const {
    return QSize(32, 32);
}

void CDatasetAttributeSet2VButton::updateOnline(ChaosBaseDatasetUI::OnlineState /*state*/) {

}

void CDatasetAttributeSet2VButton::updateValue(QVariant new_value) {
    qDebug()<< "CDatasetAttributeSet2VButton::updateValue" << deviceID() << ":" <<attributeName() << ":" << new_value;
    bool on_successfull = (new_value == QVariant(attributeSetValueOn()));
    bool off_successfull = (new_value== QVariant(attributeSetValueOff()));
    if(on_successfull || off_successfull) {
        _internal_state = on_successfull?StateOn:StateOff;
        cstate_push_button->setButtonState(on_successfull?2:1);
    } else {
        _internal_state = StateUndeterminated;
        cstate_push_button->setButtonState(0);
    }
}

void CDatasetAttributeSet2VButton::reset() {}

void CDatasetAttributeSet2VButton::changeSetCommitted() {
    qDebug()<< "CDatasetAttributeSet2VButton::changeSetCommitted" << deviceID() << ":" <<attributeName();
}

void CDatasetAttributeSet2VButton::changeSetRollback() {
    qDebug()<< "CDatasetAttributeSet2VButton::changeSetRollback" << deviceID() << ":" <<attributeName();
}

void CDatasetAttributeSet2VButton::on_pushButton_clicked(bool cliecked) {
    Q_UNUSED(cliecked)
    //change is applyed without wait commit
    switch (_internal_state) {
    case StateUndeterminated:
    case StateOff:
        //send data for on value
        emit attributeChangeSetUpdated(deviceID(),
                                       attributeName(),
                                       QVariant(attributeSetValueOn()),
                                       true);
        break;
     case StateOn:
        //send data for value off
        emit attributeChangeSetUpdated(deviceID(),
                                       attributeName(),
                                       QVariant(attributeSetValueOff()),
                                       true);
        break;
    }
}
