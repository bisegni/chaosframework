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
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //setup mds channel
    mdsChannel = chaos::ui::LLRpcApi::getInstance()->getNewMetadataServerChannel();
    trackThread = NULL;
    QObject::connect(graphWdg,SIGNAL(updatePlot()),this,SLOT(updatePlot()));
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
        model->insertRow(row++, new QStandardItem(QString((*iter).c_str())));
    }
    ui->listView->setModel(model);
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    //deviceController
    std::string attributeDescription;
    std::vector<string> attributesName;
    QString selectedDevice = ui->listView->model()->data(index, Qt::DisplayRole).toString();

    ui->label_2->setText(selectedDevice);

    std::string dName = selectedDevice.toStdString();
    deviceController.reset(chaos::ui::HLDataApi::getInstance()->getControllerForDeviceID(dName));
    deviceController->setupTracking();
    deviceController->getAttributesName(attributesName);

    QStandardItemModel *model = new QStandardItemModel(attributeDescription.size(), 2);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Description"));

    int row = 0;
    for(std::vector<std::string>::iterator iter = attributesName.begin();
        iter != attributesName.end();
        iter++){

        deviceController->getAttributeDescription((*iter), attributeDescription);
        model->setItem(row, 0, new QStandardItem(QString((*iter).c_str())));
        model->setItem(row++, 1, new QStandardItem(QString(attributeDescription.c_str())));
    }
    ui->tableView->setModel(model);

    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setResizeMode(QHeaderView::Stretch);
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(index.column() != 0) return;
    QString selectedAttribute = ui->tableView->model()->data(index, Qt::DisplayRole).toString();
    std::string attributeName =  selectedAttribute.toStdString() ;
    if(graphWdg->hasPlot(attributeName)){
        graphWdg->removePlot(attributeName);
    } else {
        deviceController->addAttributeToTrack(attributeName);
        chaos::DataBuffer *attributeBuffer = deviceController->getBufferForAttribute(attributeName);
        if(!attributeBuffer) {
            QMessageBox* msg = new QMessageBox(this);
            msg->setText("Errore getting buffer for attribute!");
            msg->show();
            return;
        }
        graphWdg->addNewPlot(attributeBuffer, attributeName);
    }
}

void MainWindow::on_buttonInit_clicked()
{
    if(!deviceController.get()) return;
    deviceController->initDevice();
    //set the scehdule delay acocrding to control
    deviceController->setScheduleDelay(ui->dialScheduleDevice->value());
}

void MainWindow::on_buttonDeinit_clicked()
{
    if(!deviceController.get()) return;
    deviceController->deinitDevice();
}

void MainWindow::on_buttonStart_clicked()
{
    if(!deviceController.get()) return;
    deviceController->startDevice();
}

void MainWindow::on_buttonStop_clicked()
{
    if(!deviceController.get()) return;
    deviceController->stopDevice();
}

void MainWindow::on_buttonStartTracking_clicked()
{
    if(trackThread) return;
    trackThread = new chaos::CThread(this);
    trackThread->setDelayBeetwenTask(1000000);
    trackThread->start();
}

void MainWindow::on_buttonStopTracking_clicked()
{
    if(!trackThread) return;
    trackThread->stop();
    delete(trackThread);
    trackThread = NULL;
}

void MainWindow::executeOnThread(const std::string&) throw(chaos::CException) {
    if(!deviceController.get()) return;
    deviceController->fetchCurrentDeviceValue();
    graphWdg->update();
}

void MainWindow::updatePlot(){
    graphWdg->replot();
}

void MainWindow::on_dialTrackSpeed_valueChanged(int value)
{
    if(!trackThread) return;
    int64_t delay(value*1000);
    trackThread->setDelayBeetwenTask(delay);
}

void MainWindow::on_dialScheduleDevice_valueChanged(int value)
{
    if(!deviceController.get()) return;
    int64_t delay(value*1000);
    deviceController->setScheduleDelay(delay);

}

void MainWindow::on_spinDeviceSchedule_valueChanged(int value)
{
    on_dialScheduleDevice_valueChanged(value);
}

void MainWindow::on_spinTrackSpeed_valueChanged(int value)
{
    on_dialTrackSpeed_valueChanged(value);
}
