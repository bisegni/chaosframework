/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include "controldialog.h"
#include "ui_controldialog.h"

#ifndef Q_MOC_RUN
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#endif
#include <qspinbox.h>
#include <limits>

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

void ControlDialog::initDialog(chaos::ui::DeviceController *_deviceController, string& _attributeName) {
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

        controlWidget = int32SpinBox;
        if(attributerange.minRange.size()==0){
          min = std::numeric_limits<int32_t>::min();
        }

        if(attributerange.maxRange.size()==0){
           max = std::numeric_limits<int32_t>::max();
        }

        if(attributerange.defaultValue.size()==0){
            def=0;
        }

        int32SpinBox->setMinimum(min);
        ui->horizontalSlider->setMinimum(min);
        int32SpinBox->setMaximum(max);
        ui->horizontalSlider->setMaximum(max);
        int32SpinBox->setValue(def);
        ui->horizontalSlider->setValue(def);
    }
        break;
    case chaos::DataType::TYPE_DOUBLE:{
        QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();

        double_t min = minStr.toDouble();
        double_t max = maxStr.toDouble();
        double_t def = defStr.toDouble();

        //QString maxValue std::max(std::abs(min),std::abs(max));
        controlWidget = doubleSpinBox;
        if(attributerange.minRange.size()==0){
          min = std::numeric_limits<double_t>::min();
        }

        if(attributerange.maxRange.size()==0){
           max = std::numeric_limits<double_t>::max();
        }

        if(attributerange.defaultValue.size()==0){
            def=0;
        }

        doubleSpinBox->setMinimum(min);
        ui->horizontalSlider->setMinimum(-100);
        doubleSpinBox->setMaximum(max);
        ui->horizontalSlider->setMaximum(100);
        doubleSpinBox->setValue(def);
        ui->horizontalSlider->setValue((int)def);
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
        int i = int32SpinBox->value();
        err = deviceController->setAttributeToValue(attributeName.c_str(), chaos::DataType::TYPE_INT32, &i);
        break;}
    case chaos::DataType::TYPE_DOUBLE:{
        QDoubleSpinBox *doubleSpinBox = (QDoubleSpinBox*)controlWidget;
        double val = doubleSpinBox->value();
        err = deviceController->setAttributeToValue(attributeName.c_str(), chaos::DataType::TYPE_DOUBLE, &val);
        break;}
    }
}

void ControlDialog::on_buttonClose_clicked()
{
    close();
}

void ControlDialog::on_horizontalSlider_sliderMoved(int position)
{
    if(controlWidget==NULL) return;
    switch(attributerange.valueType){
    case chaos::DataType::TYPE_INT32:{
        QSpinBox *int32SpinBox = (QSpinBox*)controlWidget;
        int32SpinBox->setValue(position);
        break;}
    case chaos::DataType::TYPE_DOUBLE:{
        QSpinBox *doubleSpinBox = (QSpinBox*)controlWidget;
        doubleSpinBox->setValue(position);
        break;}
    }
    on_buttonCommit_clicked();
}
