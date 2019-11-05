#include "CDatasetAttributeLabel.h"
#include <QHBoxLayout>
#include <QDebug>

CDatasetAttributeLabel::CDatasetAttributeLabel(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent) {
    labelValue = new QLabel(this);
    QHBoxLayout *layout = new QHBoxLayout();
    labelValue->setText("Dataset Value");
    layout->addWidget(labelValue);
    setLayout(layout);

//    connect(this, SIGNAL(valueChanged(QVariant)), this, SLOT(updateValue(QVariant)));
}

CDatasetAttributeLabel::~CDatasetAttributeLabel() {}

void CDatasetAttributeLabel::updateOnline(ChaosBaseDatasetUI::OnlineState /*state*/) {}

void CDatasetAttributeLabel::updateValue(QVariant new_value) {
    labelValue->setText(new_value.toString());
    qDebug() << "CDatasetAttributeLabel updated with value "<< new_value.toString();
}
