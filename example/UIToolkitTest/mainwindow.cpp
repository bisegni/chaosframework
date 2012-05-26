/*
 *	MainWindow.cpp
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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtGui/QApplication>
#include <QMessageBox>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <cmath>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <vector>
#include <string>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <controldialog.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <qevent.h>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), settings("it.infn", "CHAOS")
{
    QSettings settings;
    ui->setupUi(this);
    readSettings();

    QVBoxLayout *gL = new QVBoxLayout();
    graphWdg = new GraphWidget();
    gL->addWidget(graphWdg, 1);
    ui->graphWidget->setLayout(gL);
    ui->tableView->setEditTriggers(QAbstractItemView::EditKeyPressed);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //setup mds channel
    mdsChannel = chaos::ui::LLRpcApi::getInstance()->getNewMetadataServerChannel();
    trackThread = NULL;
    d_timerId = -1;
    lostPack = 0;
    checkSequentialIDKey.assign(ui->lineEdit->text().toStdString());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_buttonDeleteDevice_clicked(bool checked)
{
    qDebug() << "received signal";
}

void MainWindow::on_actionShow_Info_triggered()
{
    qDebug() << "Show info";
    QMessageBox* msg = new QMessageBox(this);
    msg->setText("!CHAOS Technology Test!");
    msg->show();
}

void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    settings.setValue("main_window/splitterSizes", ui->splitter->saveState());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("main_window", saveGeometry());
    settings.setValue("main_window", saveState());
    cleanLastDevice();

    QMainWindow::closeEvent(event);
}
void MainWindow::readSettings()
{
    restoreGeometry(settings.value("main_window/geometry").toByteArray());
    restoreState(settings.value("main_window/windowState").toByteArray());
    ui->splitter->restoreState(settings.value("main_window/splitterSizes").toByteArray());
}

void MainWindow::showContextMenuForWidget(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    contextMenu.addAction(new QAction(tr("&Hello"), this));
    contextMenu.exec(mapToGlobal(pos));
}

void MainWindow::on_buttonUpdateDeviceList_clicked()
{
    //update device list
    int err = 0;
    std::vector<std::string> allActiveDeviceID;
    err = mdsChannel->getAllDeviceID(allActiveDeviceID, 2000);
    if(err!=0){
        QMessageBox* msg = new QMessageBox(this);
        msg->setText("Error retriving the devicel list from metadata server");
        msg->show();
    }

    //load the device list
    QStandardItemModel *model = new QStandardItemModel(allActiveDeviceID.size(), 1);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("test header"));
    int row=0;
    for(std::vector<std::string>::iterator iter = allActiveDeviceID.begin();
        iter != allActiveDeviceID.end();
        iter++){
        model->setItem(row++, new QStandardItem(QString((*iter).c_str())));
    }
    ui->listView->setModel(model);
}



void MainWindow::updateDeviceState() {
    if(!deviceController.get()) {
        ui->labelState->setText("");
        return;
    }
    chaos::CUStateKey::ControlUnitState currentState;
    int err = deviceController->getState(currentState);
    switch(err){
        case 0:{
        switch(currentState){
        case chaos::CUStateKey::INIT:
            ui->labelState->setText("Initialized");
            break;
        case chaos::CUStateKey::DEINIT:
            ui->labelState->setText("Deinitialized");
            break;
        case chaos::CUStateKey::START:
            ui->labelState->setText("Started");
            break;
        case chaos::CUStateKey::STOP:
            ui->labelState->setText("Stopped");
            break;
        }
        break;

    default:
        ui->labelState->setText("timeout/offline");
        break;
    }
    }
}

/*
  Try to remove the last selected device
  */
void MainWindow::cleanLastDevice() {
    if(!deviceController) return;
    ui->label_2->setText("");
    ui->tableView->setModel(new QStandardItemModel(0, 4));
    //stop the possible tracking
    on_buttonStopTracking_clicked();

    //remove the plot
    graphWdg->clearAllPlot();

    //
    deviceController.reset();
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    //delete last device that is been controlled
    cleanLastDevice();

    //deviceController
    std::string attributeDescription;
    std::vector<string> attributesName;
    QString selectedDevice = ui->listView->model()->data(index, Qt::DisplayRole).toString();

    ui->label_2->setText(selectedDevice);

    std::string dName = selectedDevice.toStdString();
    deviceController.reset(chaos::ui::HLDataApi::getInstance()->getControllerForDeviceID(dName));
    deviceController->setupTracking();
    //get only output o attribute
    deviceController->getDeviceDatasetAttributesName(attributesName, chaos::DataType::Output);

    QStandardItemModel *model = new QStandardItemModel(attributeDescription.size(), 4);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Direction"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Controller"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Description"));

    int row = 0;
    for(std::vector<std::string>::iterator iter = attributesName.begin();
        iter != attributesName.end();
        iter++){
        deviceController->getAttributeDescription((*iter), attributeDescription);
        model->setItem(row, 0, new QStandardItem(QString((*iter).c_str())));
        model->setItem(row, 1, new QStandardItem("Output"));
        model->setItem(row, 2, new QStandardItem("Output"));
        model->setItem(row++, 3, new QStandardItem(QString(attributeDescription.c_str())));
    }
    //get only input o attribute
    attributesName.clear();
    deviceController->getDeviceDatasetAttributesName(attributesName, chaos::DataType::Input);
    for(std::vector<std::string>::iterator iter = attributesName.begin();
        iter != attributesName.end();
        iter++){
        deviceController->getAttributeDescription((*iter), attributeDescription);
        model->setItem(row, 0, new QStandardItem(QString((*iter).c_str())));
        model->setItem(row, 1, new QStandardItem("Input"));
        model->setItem(row, 2, new QStandardItem("text"));
        model->setItem(row++, 3, new QStandardItem(QString(attributeDescription.c_str())));
    }
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
    updateDeviceState();

    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setResizeMode(QHeaderView::Stretch);
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(index.column() != 0) return;
    QString selectedAttribute = ui->tableView->model()->data(index, Qt::DisplayRole).toString();
    std::string attributeName =  selectedAttribute.toStdString() ;
    chaos::CUSchemaDB::RangeValueInfo rangeInfo;
    // check the type of attribute
    chaos::DataType::DataSetAttributeIOAttribute direction;
    if(deviceController->getDeviceAttributeDirection(attributeName, direction)!=0){
        return;
    }

    if (direction == chaos::DataType::Output || direction == chaos::DataType::Bidirectional){
        //output
        if(graphWdg->hasPlot(attributeName)){
            graphWdg->removePlot(attributeName);
        } else {
            deviceController->addAttributeToTrack(attributeName);
            deviceController->getDeviceAttributeRangeValueInfo(attributeName, rangeInfo);
            if(rangeInfo.valueType != chaos::DataType::TYPE_BYTEARRAY){
                chaos::DataBuffer *attributeBuffer = deviceController->getBufferForAttribute(attributeName);
                if(!attributeBuffer) {
                    QMessageBox* msg = new QMessageBox(this);
                    msg->setText("Errore getting buffer for attribute!");
                    msg->show();
                    return;
                }
                graphWdg->addNewPlot(attributeBuffer, attributeName, rangeInfo.valueType);
            }else{
                chaos::PointerBuffer *pointerBuffer = deviceController->getPtrBufferForAttribute(attributeName);
                if(!pointerBuffer) {
                    QMessageBox* msg = new QMessageBox(this);
                    msg->setText("Errore getting buffer for attribute!");
                    msg->show();
                    return;
                }
                graphWdg->addNewPlot(pointerBuffer, attributeName, rangeInfo.valueType);
            }
        }
    }

    if (direction == chaos::DataType::Input || direction == chaos::DataType::Bidirectional) {
        //input attribute
        ControlDialog *ctrlDialog = new ControlDialog();
        ctrlDialog->initDialog(deviceController, attributeName);
        ctrlDialog->show();
    }
}

void MainWindow::on_buttonInit_clicked()
{
    if(!deviceController.get()) return;
    if(deviceController->initDevice()!= 0 ){
        QMessageBox* msg = new QMessageBox(this);
        msg->setText("Device already initialized or error");
        msg->show();
    }
    deviceController->setScheduleDelay(ui->dialScheduleDevice->value()*1000);
    updateDeviceState();
}

void MainWindow::on_buttonDeinit_clicked()
{
    if(!deviceController.get()) return;
    if( deviceController->deinitDevice() != 0 ){
        QMessageBox* msg = new QMessageBox(this);
        msg->setText("Device alredy deinitialized or error");
        msg->show();
    }

    updateDeviceState();
}

void MainWindow::on_buttonStart_clicked()
{
    if(!deviceController.get()) return;
    if(deviceController->startDevice() != 0 ){
        QMessageBox* msg = new QMessageBox(this);
        msg->setText("Device already started or error");
        msg->show();
    }
    updateDeviceState();
}

void MainWindow::on_buttonStop_clicked()
{
    if(!deviceController.get()) return;
    if(deviceController->stopDevice() != 0 ){
        QMessageBox* msg = new QMessageBox(this);
        msg->setText("Device already stopped or error");
        msg->show();
    }
    updateDeviceState();
}

void MainWindow::on_buttonStartTracking_clicked()
{
    if(schedThread.get()) return;
    runThread = true;
    schedThread.reset(new boost::thread(boost::bind(&MainWindow::executeOnThread, this)));

    if(d_timerId != -1) return;
    d_timerId = startTimer(40);
    lastID = 0;
    lostPack = 0;
    oversampling = 0;
    graphWdg->start();
}

void MainWindow::on_buttonStopTracking_clicked() {
    graphWdg->stop();
    if(d_timerId != -1) killTimer(d_timerId);
    d_timerId = -1;
    runThread = false;
    if(schedThread){
        schedThread->join();
        schedThread.reset();
    }
    //if(!trackThread) return;
    //trackThread->stop();
    //delete(trackThread);
    //trackThread = NULL;
}

void MainWindow::executeOnThread(){
    if(!deviceController.get()) return;
    while(runThread){
        deviceController->fetchCurrentDeviceValue();
        if(checkSequentialIDKey.size()>0){
            chaos::CDataWrapper *wrapper = deviceController->getCurrentData();
            if(wrapper->hasKey(checkSequentialIDKey.c_str())){
                int32_t curLastID = wrapper->getInt32Value(checkSequentialIDKey.c_str());
                if(lastID+1<curLastID){
                    lostPack += (curLastID - lastID + 1);
                }else if(lastID==curLastID){
                    oversampling++;
                }
                std::cout << "last id:" << lastID << " - fetchedLastID:"<<curLastID << std::endl;
                lastID = curLastID;

            }
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(ui->dialTrackSpeed->value()));
    }

}

void MainWindow::on_dialTrackSpeed_valueChanged(int value) {
}

void MainWindow::on_dialScheduleDevice_valueChanged(int value) {
    if(!deviceController.get()) return;
    deviceController->setScheduleDelay(value*1000);
}

void MainWindow::on_spinDeviceSchedule_valueChanged(int value)
{
    on_dialScheduleDevice_valueChanged(value);
}

void MainWindow::on_spinTrackSpeed_valueChanged(int value)
{
    on_dialTrackSpeed_valueChanged(value);
}

void MainWindow::on_pushButton_clicked()
{
    updateDeviceState();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == d_timerId )
    {
        QString message= "LP:";
        message.append(QString::number(lostPack));
        message.append(" os:");
        message.append(QString::number(oversampling));
        ui->lostPackageCountLabel->setText(message);
    }
    QMainWindow::timerEvent(event);
}

void MainWindow::on_spinBox_valueChanged(int points)
{
    if(graphWdg == NULL) return;
    graphWdg->setPointNumber(points);
}

void MainWindow::on_lineEdit_returnPressed()
{
    checkSequentialIDKey.assign(ui->lineEdit->text().toStdString());
}

void MainWindow::on_pushButtonResetStatistic_clicked()
{
   lostPack = 0;
   oversampling = 0;
}
