#include "ChaosUISynopticLoaderWindow.h"
#include "ui_ChaosUISynopticLoaderWindow.h"
#include "../../error/ErrorManager.h"
#include "ChaosBaseDatasetUI.h"
#include "ChaosBaseCommandButtonUI.h"
#include "ChaosBaseDatasetAttributeUI.h"
#include "ScriptSignalDialog.h"

#include <QFile>
#include <QDebug>
#include <QtCore>
#include <QByteArray>
#include <QFileDialog>
#include <QLibraryInfo>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;

ChaosUISynopticLoaderWindow::ChaosUISynopticLoaderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_edit(false),
    ui_enabled(false),
    ui(new Ui::ChaosUISynopticLoaderWindow) {
    ui->setupUi(this);
    ui->enableUIAction->setEnabled(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(customMenuRequested(QPoint)));
    qDebug() << "Form Loader path" << formLoader.pluginPaths();
    //set form search builder path
    //    formBuilder.addPluginPath(QLibraryInfo::location(QLibraryInfo::PluginsPath)+"/designer");
    //    formBuilder.addPluginPath(QProcessEnvironment::systemEnvironment().value("DYLD_LIBRARY_PATH"));

}

ChaosUISynopticLoaderWindow::~ChaosUISynopticLoaderWindow() {
    delete ui;
}

void ChaosUISynopticLoaderWindow::windowIsShown() {}

void ChaosUISynopticLoaderWindow::on_enableUIAction_triggered() {
    if(ui_enabled) {
        //disable ui
        foreach(const QString& key, map_device_root.keys()){
            monitor(key, false);
        }
        //all is gone whell change menu text
        ui->enableUIAction->setText(tr("Enable"));
        ui_enabled = false;
    } else {
        //enable ui
        foreach(const QString& key, map_device_root.keys()){
            monitor(key, true);
        }
        //all is gone whell change menu text
        ui->enableUIAction->setText(tr("Disable"));
        ui_enabled = true;
    }
    ui->loadUIFileAction->setEnabled(!ui_enabled);
    ui->editWidgetAction->setEnabled(!ui_enabled);
}

QWidget *ChaosUISynopticLoaderWindow::loadUiFile(QWidget *parent, QString filePath) {
    QWidget *result = loadAsCUF(filePath, parent);
    if(result) return result;
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    //retain compressed ui
    compressed_loaded_ui = qCompress(file.readAll(),9);

    //reset file to be loaded form ui tools
    file.reset();
    return formLoader.load(&file, parent);
}

QWidget *ChaosUISynopticLoaderWindow::loadAsCUF(QString ui_file, QWidget *parent) {
    QWidget *result_widget = nullptr;
    QFile jsonFile(ui_file);
    jsonFile.open(QFile::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
    if(doc.isNull() || !doc.isObject()) return nullptr;
    //perhaps we have a cuf file
    QJsonObject o = doc.object();
    if(!o.contains("ui")) return nullptr;
    //we have a cuf file
    //keep trak of encoded ui for nex save operation
    compressed_loaded_ui = QByteArray::fromBase64(o["ui"].toString().toUtf8());
    QByteArray uncompressed_ui = qUncompress(compressed_loaded_ui);
    QBuffer ui_description(&uncompressed_ui);
    result_widget = formLoader.load(&ui_description, parent);
    if(!result_widget) return nullptr;

    //scan all chaos_ui to deserialize setting
    QList<QWidget*> chaos_ui_widgets = result_widget->findChildren<QWidget*>();
    foreach(QWidget *ancestor, chaos_ui_widgets) {
        //serialize every chaos ui widget
        if(!ancestor->inherits("ChaosBaseUI")) continue;
        ChaosBaseUI *chaos_ui = reinterpret_cast<ChaosBaseUI*>(ancestor);
        qDebug() << QString("Deserializing %1").arg(chaos_ui->objectName());
        if(!o.contains(chaos_ui->objectName())) {
            qDebug() << QString("CUF file doesn't contains %1 derialization").arg(chaos_ui->objectName());
            continue;
        }

        //deserialize chaos widget
        QJsonObject widget_ser = o[chaos_ui->objectName()].toObject();
        chaos_ui->deserialize(widget_ser);
    }
    return result_widget;
}


void ChaosUISynopticLoaderWindow::on_saveUIFileAction_triggered() {
    QString fname = QFileDialog::getSaveFileName(this, tr("File destination"), QDir::homePath(), "User Interface (*.cuf)" );
    QFileInfo file_info(fname);
    fname = file_info.absolutePath()+"/"+file_info.fileName()+".cuf";
    QFile file(fname);
    file.open(QIODevice::WriteOnly);
    //QDataStream stream( &file );
    //save chaos file ui formar cuf(Chaos Ui Format)
    QJsonObject chaos_dataset_config;
    //add ui
    chaos_dataset_config.insert("ui", QString(compressed_loaded_ui.toBase64()));
    //add chaos widget attribute
    QList<QWidget*> chaos_ui_widgets = centralWidget()->findChildren<QWidget*>();
    foreach(QWidget *ancestor, chaos_ui_widgets) {
        //serialize every chaos ui widget
        if(!ancestor->inherits("ChaosBaseUI")) continue;
        ChaosBaseUI *chaos_ui = reinterpret_cast<ChaosBaseUI*>(ancestor);

        chaos_dataset_config.insert(chaos_ui->objectName(), QJsonValue(chaos_ui->serialize()));
    }
    QJsonDocument doc(chaos_dataset_config);
    //write file
    file.write(doc.toJson(QJsonDocument::Indented));
}

void ChaosUISynopticLoaderWindow::on_loadUIFileAction_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open ui file"),
                                                    QDir::homePath(),
                                                    tr("Ui Files (*.ui);; Chaos User Interface (*.cuf *.CUF)"));
    if(!fileName.size()) {
        return;
    }

    //allocate js engine for new ui
    if(js_engine.data() != nullptr) {
        js_engine->collectGarbage();
    }
    js_engine.reset(new QJSEngine());
    js_engine->installExtensions(QJSEngine::ConsoleExtension);
    qDebug() << "load file:" << fileName;
    //we can load ui file

    QWidget *rootUIWidget = loadUiFile(this, fileName);
    setCentralWidget(rootUIWidget);

    //update internal structure to improve performance
    QList<QWidget*> chaos_ui_widgets = centralWidget()->findChildren<QWidget*>();
    foreach(QWidget *ancestor, chaos_ui_widgets) {
        //register for command
        if(ancestor->inherits("ChaosBaseCommandButtonUI")) {
            ChaosBaseCommandButtonUI *command_ui = reinterpret_cast<ChaosBaseCommandButtonUI*>(ancestor);
            QObject::connect(command_ui,
                             &ChaosBaseCommandButtonUI::commitChangeSet,
                             this,
                             &ChaosUISynopticLoaderWindow::commitChangeSet);
            QObject::connect(command_ui,
                             &ChaosBaseCommandButtonUI::rollbackChangeSet,
                             this,
                             &ChaosUISynopticLoaderWindow::rollbackChangeSet);
        } else if(ancestor->inherits("ChaosBaseDatasetUI")) {
            ChaosBaseDatasetUI *cw = reinterpret_cast<ChaosBaseDatasetUI*>(ancestor);

            if(cw->deviceID() == nullptr ||
                    cw->deviceID().compare("") == 0) {continue;}

            //-------add object to script engine;
            js_engine->globalObject().setProperty(cw->objectName(), js_engine->newQObject(cw));
            //-------add object to script engine;

            QSharedPointer<CUNodeRoot> device_root;
            if(map_device_root.contains(cw->deviceID())) {
                device_root = map_device_root.find(cw->deviceID()).value();
            } else {
                map_device_root.insert(cw->deviceID(), device_root = QSharedPointer<CUNodeRoot>(new CUNodeRoot(cw->deviceID())));
            }
            qDebug() << "Found chaos widget " << cw->objectName() << " for device id " << cw->deviceID();
            //connect root signal to widget slots
            //online state
            QObject::connect(device_root.data(),
                             &CUNodeRoot::updateOnlineState,
                             cw,
                             &ChaosBaseDatasetUI::updateOnlineStateSlot);

            //dataset update
            QObject::connect(device_root.data(),
                             &CUNodeRoot::updateDatasetAttribute,
                             cw,
                             &ChaosBaseDatasetUI::updateData);

            //register for change-set
            if(ancestor->inherits("ChaosBaseDatasetAttributeUI") == true) {
                ChaosBaseDatasetAttributeUI *attr_ui = reinterpret_cast<ChaosBaseDatasetAttributeUI*>(ancestor);
                //connect root signal to ui
                QObject::connect(device_root.data(),
                                 &CUNodeRoot::changeSetCommitted,
                                 attr_ui,
                                 &ChaosBaseDatasetAttributeUI::changeSetCommitted);
                QObject::connect(device_root.data(),
                                 &CUNodeRoot::changeSetRollback,
                                 attr_ui,
                                 &ChaosBaseDatasetAttributeUI::changeSetRollback);
                //conenct ui signal to root
                QObject::connect(attr_ui,
                                 &ChaosBaseDatasetAttributeUI::attributeChangeSetUpdated,
                                 device_root.data(),
                                 &CUNodeRoot::attributeChangeSetUpdated);
                QObject::connect(attr_ui,
                                 &ChaosBaseDatasetAttributeUI::attributeChangeSetClear,
                                 device_root.data(),
                                 &CUNodeRoot::attributeChangeSetClear);
            }
        }
    }
    ui->enableUIAction->setEnabled(true);

    applyFunctionToChaosBaseWidget([](ChaosBaseDatasetUI *wgdt) -> void {
        QMetaObject::invokeMethod(wgdt,
                                  "chaosWidgetEditMode",
                                  Qt::QueuedConnection,
                                  Q_ARG(bool, false));
    });
}

void ChaosUISynopticLoaderWindow::on_editWidgetAction_triggered() {
    if(ui_enabled) return;
    if(ui_edit) {
        //all is gone whell change menu text
        ui->editWidgetAction->setText(tr("Edit Mode"));
        ui_edit = false;
    } else {
        //all is gone whell change menu text
        ui->editWidgetAction->setText(tr("Run Mode"));
        ui_edit = true;
    }
    //set edit mode on all chaos widget
    ui->enableUIAction->setEnabled(!ui_edit);
    QList<QWidget*> chaos_ui_widgets = centralWidget()->findChildren<QWidget*>();
    applyFunctionToChaosBaseWidget([this](ChaosBaseDatasetUI *wgdt) -> void {
        QMetaObject::invokeMethod(wgdt,
                                  "chaosWidgetEditMode",
                                  Qt::QueuedConnection,
                                  Q_ARG(bool, ui_edit));
    });
}

void ChaosUISynopticLoaderWindow::customMenuRequested(QPoint point) {
    QWidget *child_at_position = childAt(point);
    ChaosBaseDatasetUI *cw  = getChaosWidgetParent(child_at_position);
    if(cw == nullptr) return;
    QMenu *menu=new QMenu(this);
    QAction *ac1 = new QAction(QString("Edit script for %1").arg(cw->objectName()), this);
    ac1->setData(QVariant(cw->objectName()));
    connect(ac1,
            SIGNAL(triggered()),
            this,
            SLOT(editScript()));
    menu->addAction(ac1);
    menu->popup(centralWidget()->mapToGlobal(point));
}

void ChaosUISynopticLoaderWindow::editScript() {
    QAction* pAction = qobject_cast<QAction*>(sender());
    Q_ASSERT(pAction);
    QWidget *chaos_ui = centralWidget()->findChild<QWidget*>(pAction->data().toString());
    if(chaos_ui == nullptr) return;

    ScriptSignalDialog scriptDialog(chaos_ui);
    scriptDialog.setWindowTitle(tr("Script Editor"));
    if(scriptDialog.exec() == QDialog::Accepted) {
        //install cript
        QObject::disconnect(chaos_ui,nullptr,nullptr,nullptr);

        //re-install all script on event
        QMap<QString, QSharedPointer<StructSignalFunction> > compiledScript = scriptDialog.getScript();
        auto iter = compiledScript.begin();
        while(iter != compiledScript.end()) {
            //compile the jslsot
            qDebug() << "Compile script:\n" << iter.value()->script_source;
            QJSValue compiled_function = js_engine->evaluate(iter.value()->script_source);
            if(compiled_function.isError()) {
                qDebug()<< "\nUncaught exception at line"
                        << compiled_function.property("lineNumber").toInt()
                        << ":" << compiled_function.toString();

            } else {
                qDebug() << "Attach function:\n" << iter.value()->function_name << "to signla " << iter.value()->signal_name << " of widget "<< chaos_ui->objectName();
                QString js_connection = QString("%1.%2.connect(%3)").arg(chaos_ui->objectName()).arg(iter.value()->signal_name).arg(iter.value()->function_name);
                QJSValue compiled_connection = js_engine->evaluate(js_connection);
                if(compiled_connection.isError()) {
                    qDebug()<< "\nUncaught exception at line"
                            << compiled_connection.property("lineNumber").toInt()
                            << ":" << compiled_connection.toString();
                }
            }
            iter++;
        }
    }
}

void ChaosUISynopticLoaderWindow::nodeChangedOnlineState(const std::string& control_unit_uid,
                                                         OnlineState old_state,
                                                         OnlineState  new_state) {
    Q_UNUSED(old_state)
    qDebug() << "update oline state for " << QString::fromStdString(control_unit_uid) << " new_state "<< new_state;
    auto cu_root_iterator = map_device_root.find(QString::fromStdString(control_unit_uid));
    if(cu_root_iterator == map_device_root.end()) return;
    //signal to root cu
    cu_root_iterator.value()->setOnlineState(static_cast<ChaosBaseDatasetUI::OnlineState>(new_state));
}

void ChaosUISynopticLoaderWindow::nodeChangedInternalState(const std::string& control_unit_uid,
                                                           const std::string& old_status,
                                                           const std::string& new_status) {
    Q_UNUSED(old_status)
    //    qDebug() << "updatedDS for " << QString::fromStdString(control_unit_uid);
    auto cu_root_iterator = map_device_root.find(QString::fromStdString(control_unit_uid));
    if(cu_root_iterator == map_device_root.end()) return;
    cu_root_iterator.value()->setCurrentAttributeValue(chaos::DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH,
                                                       QString::fromStdString(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS),
                                                       QVariant(QString::fromStdString(new_status)));
}

void ChaosUISynopticLoaderWindow::nodeChangedProcessResource(const std::string& control_unit_uid,
                                                             const ProcessResource& old_proc_res,
                                                             const ProcessResource& new_proc_res) {
    Q_UNUSED(control_unit_uid)
    Q_UNUSED(old_proc_res)
    Q_UNUSED(new_proc_res)
    //    qDebug() << QString("%5 nodeChangedProcessResource: usr:%1 sys:%2 swp:%3 upt:%4").arg(new_proc_res.usr_res).arg(new_proc_res.sys_res).arg(new_proc_res.swp_res).arg(new_proc_res.uptime).arg(QString::fromStdString(control_unit_uid));

}

void ChaosUISynopticLoaderWindow::nodeChangedErrorInformation(const std::string& control_unit_uid,
                                                              const ErrorInformation& old_status,
                                                              const ErrorInformation& new_status) {
    Q_UNUSED(old_status)
    Q_UNUSED(new_status)
    qDebug()<< QString::fromStdString(control_unit_uid) << "nodeChangedErrorInformation: ";

}

void ChaosUISynopticLoaderWindow::updatedDS(const std::string& control_unit_uid,
                                            int dataset_type,
                                            MapDatasetKeyValues& dataset_key_values) {
    //    qDebug() << "updatedDS for " << QString::fromStdString(control_unit_uid) << " ds_type "<< dataset_type << "attributes:"<<dataset_key_values.size();
    auto cu_root_iterator = map_device_root.find(QString::fromStdString(control_unit_uid));
    if(cu_root_iterator == map_device_root.end()) return;

    //signal every update to root cu
    for (MapDatasetKeyValuesPair element : dataset_key_values) {
        cu_root_iterator.value()->setCurrentAttributeValue(dataset_type,
                                                           QString::fromStdString(element.first),
                                                           toQVariant(element.second));
    }
}

void ChaosUISynopticLoaderWindow::noDSDataFound(const std::string& control_unit_uid,
                                                int dataset_type) {
    Q_UNUSED(control_unit_uid)
    Q_UNUSED(dataset_type)
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
        result.setValue(QString::fromStdString(chaos_value.asString()));
        break;
    }
    case DataType::DataType::TYPE_BOOLEAN: {
        result.setValue(chaos_value.asBool());
        break;
    }
    case DataType::DataType::TYPE_BYTEARRAY: {
        ChaosSharedPtr<CDataBuffer> shared_buffer = chaos_value.asCDataBufferShrdPtr();
        result.setValue(QByteArray::fromRawData(shared_buffer->getBuffer(), shared_buffer->getBufferSize()));
        break;
    }
    default:
        break;
    }
    return result;
}

void ChaosUISynopticLoaderWindow::closeEvent(QCloseEvent *event) {
    //disable ui
    foreach(const QString& key, map_device_root.keys()){
        monitor(key, false);
    }
    QMainWindow::closeEvent(event);
}

ChaosBaseDatasetUI *ChaosUISynopticLoaderWindow::getChaosWidgetParent(QObject *w) {
    QObject *cur = w;
    ChaosBaseDatasetUI *result = nullptr;
    if(!w) return nullptr;
    do {
        if(cur->inherits("ChaosBaseDatasetUI")) {
            result = reinterpret_cast<ChaosBaseDatasetUI*>(cur);
        } else {
            cur = cur->parent();
        }
    }while(cur && !result);
    return result;
}

void ChaosUISynopticLoaderWindow::commitChangeSet() {
    QMapIterator<QString, QSharedPointer<CUNodeRoot> > i(map_device_root);
    while (i.hasNext()) {
        i.next();
        qDebug() << "Apply change set to:" << i.key();
        QMetaObject::invokeMethod(i.value().get(),
                                  "commitChangeSet",
                                  Qt::QueuedConnection);
    }
}

void ChaosUISynopticLoaderWindow::rollbackChangeSet() {
    QMapIterator<QString, QSharedPointer<CUNodeRoot> > i(map_device_root);
    while (i.hasNext()) {
        i.next();
        qDebug() << "Apply rollback to:" << i.key();
        QMetaObject::invokeMethod(i.value().get(),
                                  "rollbackChangeSet",
                                  Qt::QueuedConnection);
    }
}

//private
void ChaosUISynopticLoaderWindow::applyFunctionToChaosBaseWidget(std::function<void(ChaosBaseDatasetUI*)>  function) {
    QList<QWidget*> chaos_ui_widgets = centralWidget()->findChildren<QWidget*>();
    foreach(QWidget *ancestor, chaos_ui_widgets) {
        if(ancestor->inherits("ChaosBaseDatasetUI") == false) {continue;}

        ChaosBaseDatasetUI *cw = reinterpret_cast<ChaosBaseDatasetUI*>(ancestor);
        function(cw);
    }
}
