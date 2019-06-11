#include "ChaosUISynopticLoaderWindow.h"
#include "ui_ChaosUISynopticLoaderWindow.h"
#include "../../error/ErrorManager.h"
#include "ChaosBaseDatasetUI.h"

#include <QFile>
#include <QtUiTools>
#include <QFileDialog>
#include <QDebug>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;

ChaosUISynopticLoaderWindow::ChaosUISynopticLoaderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_enabled(false),
    api_submitter(this),
    ui(new Ui::ChaosUISynopticLoaderWindow) {
    ui->setupUi(this);
    ui->enableUIAction->setEnabled(false);
}

ChaosUISynopticLoaderWindow::~ChaosUISynopticLoaderWindow()
{
    delete ui;
}

QWidget *ChaosUISynopticLoaderWindow::loadUiFile(QWidget *parent, QString filePath) {
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);

    QUiLoader loader;
    return loader.load(&file, parent);
}

void ChaosUISynopticLoaderWindow::windowIsShown() {

}

void ChaosUISynopticLoaderWindow::on_enableUIAction_triggered() {
    if(ui_enabled) {
        //disable ui
        foreach(const QString& key, hash_device_root.keys()){
            monitor(key, false);
        }
        //all is gone whell change menu text
        ui->enableUIAction->setText(tr("Enable"));
        ui_enabled = false;
    } else {
        //enable ui
        foreach(const QString& key, hash_device_root.keys()){
            monitor(key, true);
        }
        //all is gone whell change menu text
        ui->enableUIAction->setText(tr("Disable"));
        ui_enabled = true;
    }
    ui->loadUIFileAction->setEnabled(!ui_enabled);
}

void ChaosUISynopticLoaderWindow::on_loadUIFileAction_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open ui file"),
                                                    "$HOME",
                                                    tr("Ui Files (*.ui)"));
    if(!fileName.size()) {
        return;
    }
    qDebug() << "load file:" << fileName;
    //we can load ui file

    QWidget *rootUIWidget = loadUiFile(this, fileName);
    setCentralWidget(rootUIWidget);

    //update internal structure to improve performance
    QList<QWidget*> chaos_ui_widgets = centralWidget()->findChildren<QWidget*>();
    foreach(QWidget *ancestor, chaos_ui_widgets) {
        if(ancestor->inherits("ChaosBaseDatasetUI") == false) {continue;}

        ChaosBaseDatasetUI *cw = reinterpret_cast<ChaosBaseDatasetUI*>(ancestor);

        if(cw->deviceID() == nullptr ||
                cw->deviceID().compare("") == 0) {continue;}

        QSharedPointer<CUNodeRoot> device_root;
        if(hash_device_root.contains(cw->deviceID())) {
            device_root = hash_device_root.find(cw->deviceID()).value();
        } else {
            hash_device_root.insert(cw->deviceID(), device_root = QSharedPointer<CUNodeRoot>(new CUNodeRoot(cw->deviceID())));
        }
        qDebug() << "Found chaos widget " << cw->objectName() << " for device id " << cw->deviceID();
        QObject::connect(device_root.data(),
                         SIGNAL(updateDatasetAttribute(QString, QVariant)),
                         cw,
                         SLOT(updateData(QString, QVariant)));
//        cu_object_hash.insert(cw->deviceID(), cw);
    }
    ui->enableUIAction->setEnabled(true);

    //    QMetaObject::invokeMethod(this, "windowIsShown", Qt::QueuedConnection);
}



void ChaosUISynopticLoaderWindow::nodeChangedOnlineState(const std::string& /*node_uid*/,
                                                         OnlineState /*old_status*/,
                                                         OnlineState  /*new_status*/) {
    //    online_status = new_status;
    //    QMetaObject::invokeMethod(this,
    //                              "updateUIStatus",
    //                              Qt::QueuedConnection);
}

void ChaosUISynopticLoaderWindow::updatedDS(const std::string& control_unit_uid,
                                            int dataset_type,
                                            MapDatasetKeyValues& dataset_key_values) {
    qDebug() << "updatedDS for " << QString::fromStdString(control_unit_uid) << " ds_type "<< dataset_type << "attributes:"<<dataset_key_values.size();
    auto cu_root_iterator = hash_device_root.find(QString::fromStdString(control_unit_uid));
    if(cu_root_iterator == hash_device_root.end()) return;

    for (MapDatasetKeyValuesPair element : dataset_key_values) {
        cu_root_iterator.value()->setCurrentAttributeValue(QString::fromStdString(element.first),
                                                           toQVariant(element.second));
    }
}

void ChaosUISynopticLoaderWindow::noDSDataFound(const std::string& /*control_unit_uid*/,
                                                int /*dataset_type*/) {
    //    data_found = false;
    //    storage_type = DSStorageTypeUndefined;
    //    QMetaObject::invokeMethod(this,
    //                              "updateUIStatus",
    //                              Qt::QueuedConnection);
}

bool ChaosUISynopticLoaderWindow::monitor(const QString& cu_uid,
                                          bool enable) {
    bool result = true;
    if(enable) {
        result = ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(cu_uid.toStdString(),
                                                                                    chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit,
                                                                                    this);
    } else {
        result = ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(cu_uid.toStdString(),
                                                                                       chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit,
                                                                                       this);
    }
    return result;
}

void ChaosUISynopticLoaderWindow::onApiDone(const QString& /*tag*/,
                                            QSharedPointer<chaos::common::data::CDataWrapper> /*api_result*/) {

}
void ChaosUISynopticLoaderWindow::onApiError(const QString& /*tag*/,
                                             QSharedPointer<chaos::CException> api_exception) {
    ErrorManager::getInstance()->submiteError(api_exception);
}
void ChaosUISynopticLoaderWindow::onApiTimeout(const QString& /*tag*/) {}
void ChaosUISynopticLoaderWindow::apiHasStarted(const QString& /*api_tag*/) {}
void ChaosUISynopticLoaderWindow::apiHasEnded(const QString& /*api_tag*/) {}
void ChaosUISynopticLoaderWindow::apiHasEndedWithError(const QString& /*tag*/,
                                                       const QSharedPointer<chaos::CException> /*api_exception*/){}

QVariant ChaosUISynopticLoaderWindow::toQVariant(chaos::common::data::CDataVariant chaos_value) {
    QVariant result;
    switch (chaos_value.getType()) {
    case DataType::DataType::TYPE_INT32: {
        result.setValue(chaos_value.asInt32());
        break;
    }
    case DataType::DataType::TYPE_INT64: {
        result.setValue(chaos_value.asInt64());
        break;
    }
    case DataType::DataType::TYPE_DOUBLE: {
        result.setValue(chaos_value.asDouble());
        break;
    }
    case DataType::DataType::TYPE_STRING: {
        result.setValue(chaos_value.asString());
        break;
    }
    case DataType::DataType::TYPE_BOOLEAN: {
        result.setValue(chaos_value.asBool());
        break;
    }
    case DataType::DataType::TYPE_BYTEARRAY: {
        ChaosSharedPtr<CDataBuffer> shared_buffer = chaos_value.asCDataBufferShrdPtr();
        result.setValue(shared_buffer);
        break;
    }
    default:
        break;
    }
    return result;
}

void ChaosUISynopticLoaderWindow::closeEvent(QCloseEvent *event) {
    //disable ui
    foreach(const QString& key, hash_device_root.keys()){
        monitor(key, false);
    }
    QMainWindow::closeEvent(event);
}
