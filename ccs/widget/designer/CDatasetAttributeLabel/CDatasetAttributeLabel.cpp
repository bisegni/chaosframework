#include "CDatasetAttributeLabel.h"
#include <QHBoxLayout>

CDatasetAttributeLabel::CDatasetAttributeLabel(QWidget *parent) :
    ChaosBaseDatasetUI(parent) {
    internalLabel = new QLabel(this);
    QHBoxLayout *layout = new QHBoxLayout();
    internalLabel->setText("Dataset Value");
    layout->addWidget(internalLabel);
    setLayout(layout);
}

CDatasetAttributeLabel::~CDatasetAttributeLabel() {}

void CDatasetAttributeLabel::updateValue(QVariant variant_value) {
    internalLabel->setText(variant_value.toString());
}
