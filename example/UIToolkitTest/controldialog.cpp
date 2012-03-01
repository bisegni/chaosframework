/*
 *	ControlDialog.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
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
    case chaos::DataType::TYPE_INT32:{
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
        }}
        break;
    case chaos::DataType::TYPE_DOUBLE:{
        QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();

        double_t min = minStr.toDouble();
        double_t max = maxStr.toDouble();
        double_t def = defStr.toDouble();

        //QString maxValue std::max(std::abs(min),std::abs(max));
        controlWidget = doubleSpinBox;
        if(attributerange.minRange.size()>0){
            doubleSpinBox->setMinimum(min);
        }
        if(attributerange.maxRange.size()>0){
            doubleSpinBox->setMaximum(max);
        }

        if(attributerange.defaultValue.size()>0){
            doubleSpinBox->setValue(def);
        }
        break;
    }}
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
    case chaos::DataType::TYPE_INT32:{
        QSpinBox *int32SpinBox = (QSpinBox*)controlWidget;
        err = deviceController->setInt32AttributeValue(attributeName, int32SpinBox->value());
        break;}
    case chaos::DataType::TYPE_DOUBLE:{
        QSpinBox *doubleSpinBox = (QSpinBox*)controlWidget;
        err = deviceController->setDoubleAttributeValue(attributeName, doubleSpinBox->value());
        break;}
    }
}

void ControlDialog::on_buttonClose_clicked()
{
    close();
}
