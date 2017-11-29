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

#include "SelectNetworkDomain.h"
#include "ui_SelectNetworkDomain.h"
#include "PreferenceManager.h"

#include <QMessageBox>

SelectNetworkDomain::SelectNetworkDomain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectNetworkDomain) {
    ui->setupUi(this);
    loadAllConfiguration();
}

SelectNetworkDomain::~SelectNetworkDomain() {
    delete ui;
}

void SelectNetworkDomain::loadAllConfiguration() {
    //write current configurations
    ui->comboBoxConfigurations->clear();
    ui->comboBoxConfigurations->addItems(PreferenceManager::getInstance()->getNetworkConfigurationNames());
    ui->comboBoxConfigurations->setCurrentText(PreferenceManager::getInstance()->getActiveNetworkConfigurationName());
}

void SelectNetworkDomain::on_pushButtonCancel_clicked() {
    //check configuration
    bool configured = false;
    try{
        configured = PreferenceManager::getInstance()->activeNetworkConfiguration(ui->comboBoxConfigurations->currentText());
        if(configured) {
            emit networkDomainSelected(ui->comboBoxConfigurations->currentText());
            accept();
        }
    } catch(chaos::CException& ex) {

    } catch(...) {

    }
    if(configured == false) {
        QMessageBox::critical(this,
                              "Configuraiton check",
                              "Configuraiton has failede plese try onether one");
    }
}

void SelectNetworkDomain::on_pushButtonSelection_clicked() {
    emit selectionAborted();
    reject();
}
