#include "CDatasetAttributeLabel.h"
#include <QHBoxLayout>
#include <QDebug>

CDatasetAttributeLabel::CDatasetAttributeLabel(QWidget *parent) :
    ChaosBaseDatasetUI(parent) {
    labelValue = new QLabel(this);
    QHBoxLayout *layout = new QHBoxLayout();
    labelValue->setText("Dataset Value");
    layout->addWidget(labelValue);
    setLayout(layout);

//    connect(this, SIGNAL(valueChanged(QVariant)), this, SLOT(updateValue(QVariant)));
}

CDatasetAttributeLabel::~CDatasetAttributeLabel() {}

void CDatasetAttributeLabel::updateValue(QVariant variant_value) {
    labelValue->setText(variant_value.toString());
    qDebug() << "CDatasetAttributeLabel updated with value "<< variant_value.toString();
}
