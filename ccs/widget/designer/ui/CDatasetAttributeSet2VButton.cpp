#include "CDatasetAttributeSet2VButton.h"

#include <QDebug>
#include <QDateTime>
#include <QHBoxLayout>

CDatasetAttributeSet2VButton::CDatasetAttributeSet2VButton(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent){
    cstate_push_button = new CStatePushButton(this);
    cstate_push_button->addState(CStatePushButton::StateInfo("Disabled", Qt::lightGray));
    cstate_push_button->addState(CStatePushButton::StateInfo("Enable", Qt::green));
    cstate_push_button->addState(CStatePushButton::StateInfo("In Error", Qt::red));
    cstate_push_button->setCheckable(true);
    connect(cstate_push_button,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(-1);
    layout->setMargin(-1);
    layout->addWidget(cstate_push_button);
    setLayout(layout);
}

QString CDatasetAttributeSet2VButton::attributeSetValue() {
    return p_attribute_setValue;
}

void CDatasetAttributeSet2VButton::setAttributeSetValue(QString& new_p_attribute_setValue) {
    p_attribute_setValue = new_p_attribute_setValue;
}

QSize CDatasetAttributeSet2VButton::sizeHint() const {
    return QSize(16, 16);
}

QSize CDatasetAttributeSet2VButton::minimumSizeHint() const {
    return QSize(16, 16);
}

void CDatasetAttributeSet2VButton::updateOnline(ChaosBaseDatasetUI::OnlineState /*state*/) {

}

void CDatasetAttributeSet2VButton::updateValue(QVariant new_value) {
    bool set_successfull = new_value.toString().compare(attributeSetValue())==0;
    cstate_push_button->setButtonState(set_successfull);
    cstate_push_button->setChecked(set_successfull);
}

void CDatasetAttributeSet2VButton::changeSetCommitted() {
    qDebug()<< "CDatasetAttributeSet2VButton::changeSetCommitted" << deviceID() << ":" <<attributeName();
}


void CDatasetAttributeSet2VButton::on_pushButton_clicked(bool cliecked) {
    Q_UNUSED(cliecked)
    cstate_push_button->setChecked(false);
    emit attributeChangeSetUpdated(deviceID(),
                                   attributeName(),
                                   QVariant(attributeSetValue()));
}
