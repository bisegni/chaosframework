#include "controldialog.h"
#include "ui_controldialog.h"
#include <qspinbox.h>
ControlDialog::ControlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlDialog)
{
    controlWidget = NULL;
    ui->setupUi(this);
}

ControlDialog::~ControlDialog()
{
    delete ui;
}

void ControlDialog::initDialog(boost::shared_ptr<chaos::ui::DeviceController>&  _deviceController, string& _attributeName) {
    deviceController = _deviceController;
    attributeName = _attributeName;
    //get the attribute info and allocate the controller
    deviceController->getDeviceId(deviceID);
    deviceController->getDeviceAttributeRangeValueInfo(attributeName, attributerange);

    ui->label->setText(QString(attributeName.c_str()));

    QString minStr(attributerange.minRange.c_str());
    QString maxStr(attributerange.maxRange.c_str());
    QString defStr(attributerange.defaultValue.c_str());

    switch(attributerange.valueType){
        case chaos::DataType::TYPE_INT32:
        QSpinBox *int32SpinBox = new QSpinBox();

        int32_t min = minStr.toInt();
        int32_t max = maxStr.toInt();
        int32_t def = defStr.toInt();

        //QString maxValue std::max(std::abs(min),std::abs(max));

        controlWidget = int32SpinBox;
        if(attributerange.minRange.size()>0){
            int32SpinBox->setMinimum(min);
        }

        if(attributerange.maxRange.size()>0){
            int32SpinBox->setMaximum(max);
        }

        if(attributerange.defaultValue.size()>0){
            int32SpinBox->setValue(def);
        }
        break;
    }
    if(controlWidget == NULL) return;

    QVBoxLayout *gL = new QVBoxLayout();
    gL->addWidget(controlWidget, 1);
    ui->widget->setLayout(gL);
}

void ControlDialog::on_buttonCommit_clicked()
{
    int err = 0;
    if(controlWidget==NULL) return;
    switch(attributerange.valueType){
        case chaos::DataType::TYPE_INT32:
        QSpinBox *int32SpinBox = (QSpinBox*)controlWidget;
        err = deviceController->setInt32AttributeValue(attributeName, int32SpinBox->value());
        break;
    }
}

void ControlDialog::on_buttonClose_clicked()
{
    close();
}
