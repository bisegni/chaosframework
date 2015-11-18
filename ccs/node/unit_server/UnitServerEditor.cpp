#include "UnitServerEditor.h"
#include "ui_UnitServerEditor.h"
#include "../control_unit/ControUnitInstanceEditor.h"
#include "../control_unit/ControlUnitEditor.h"

#include <chaos/common/data/CDataWrapper.h>

#include <QDebug>
#include <QPointer>
#include <QDateTime>
#include <QMessageBox>
#include <QModelIndex>
#include <QMenu>
#include <QMessageBox>
#include <QTimeZone>
#include <QLocale>
#include <QInputDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QStringRef>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

const QString TAG_CU_ADD_NEW_TYPE = "tag_cu_new_type";
const QString TAG_CU_REMOVE_TYPE = "tag_cu_remove_type";
const QString TAG_CU_REMOVE_TYPE_AND_UPDATE_LIST = "tag_cu_remove_type_ul";
const QString TAG_GET_DESCRIPTION = "get_description";
const QString TAG_CU_SI = "cu_si";

UnitServerEditor::UnitServerEditor(const QString &_node_unique_id) :
    PresenterWidget(NULL),
    node_unique_id(_node_unique_id),
    move_copy_search_instance(NULL),
    ui(new Ui::UnitServerEditor) {
    ui->setupUi(this);
    ui->splitterTypeInstances->setStretchFactor(0,0);
    ui->splitterTypeInstances->setStretchFactor(1,1);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,
            SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(customMenuRequested(QPoint)));

}

UnitServerEditor::~UnitServerEditor() {
    delete ui;
}

void UnitServerEditor::initUI() {
    setTabTitle(node_unique_id);
    ui->labelUnitServerUID->setText(node_unique_id);

    //create cu type list model
    list_model_cu_type = new QStringListModel(this);
    ui->listViewCUType->setModel(list_model_cu_type);

    //connect update button to interla slot
    connect(ui->pushButtonUpdateAllInfo, SIGNAL(clicked()), this, SLOT(updateAll()));

    // Create a new model
    // QStandardItemModel(int rows, int columns, QObject * parent = 0)
    table_model = new QStandardItemModel(this);
    table_model->setHorizontalHeaderItem(0, new QStandardItem(QString("UID")));
    table_model->setHorizontalHeaderItem(1, new QStandardItem(QString("Impl")));
    table_model->setHorizontalHeaderItem(2, new QStandardItem(QString("RPC Addr")));
    table_model->setHorizontalHeaderItem(3, new QStandardItem(QString("RPC Dom")));


    // Attach the model to the view
    ui->tableView->setModel(table_model);

    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //set connection
    connect(ui->listViewCUType->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(handleSelectionChanged(QItemSelection)));

    ui->tableView->setAcceptDrops(true);

    connect(ui->tableView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(tableCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->tableView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableSelectionChanged(QItemSelection,QItemSelection)));

    //setup chaos ui
    ui->chaosLabelHealtStatus->setNodeUniqueID(node_unique_id);
    ui->chaosLabelHealtStatus->setTrackStatus(true);
    ui->chaosLabelHealtStatus->setLabelValueShowTrackStatus(true);
    ui->chaosLedIndicatorHealt->setNodeUniqueID(node_unique_id);
    connect(ui->chaosLedIndicatorHealt,
            SIGNAL(changedOnlineStatus(QString,CLedIndicatorHealt::AliveState)),
            SLOT(changedNodeOnlineStatus(QString,CLedIndicatorHealt::AliveState)));
    //start monitor on chaos ui
    ui->chaosLabelHealtStatus->startMonitoring();
    ui->chaosLedIndicatorHealt->startMonitoring();
    //load info
    updateAll();
}

bool UnitServerEditor::isClosing() {
    //stop monitoring
    ui->chaosLabelHealtStatus->stopMonitoring();
    ui->chaosLedIndicatorHealt->stopMonitoring();
    if( move_copy_search_instance) {
        move_copy_search_instance->close();
        delete( move_copy_search_instance);
        move_copy_search_instance = NULL;
    }
    return true;
}

void UnitServerEditor::customMenuRequested(QPoint pos){
    //QModelIndex index=ui->tableView->indexAt(pos);
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if(selected.size()) {
        QMenu *menu=new QMenu(this);
        QAction *menuL = new QAction("Load", this);
        QAction *menuUL = new QAction("Unload", this);
        QAction *menuI = new QAction("Init", this);
        QAction *menuDI = new QAction("Deinit", this);
        QAction *menuStart = new QAction("Start", this);
        QAction *menuStop = new QAction("Stop", this);
        QAction *menuDuplicate = new QAction("Duplicate", this);
        QAction *menuCopyTo = new QAction("Copy To", this);
        QAction *menuMoveTo = new QAction("Move To", this);
        connect(menuL, SIGNAL(triggered()), this, SLOT(cuInstanceLoadSelected()));
        connect(menuUL, SIGNAL(triggered()), this, SLOT(cuInstanceUnloadSelected()));
        connect(menuI, SIGNAL(triggered()), this, SLOT(cuInstanceInitSelected()));
        connect(menuDI, SIGNAL(triggered()), this, SLOT(cuInstanceDeinitSelected()));
        connect(menuStart, SIGNAL(triggered()), this, SLOT(cuInstanceStartSelected()));
        connect(menuStop, SIGNAL(triggered()), this, SLOT(cuInstanceStopSelected()));
        connect(menuDuplicate, SIGNAL(triggered()), this, SLOT(duplicateInstance()));
        connect(menuCopyTo, SIGNAL(triggered()), this, SLOT(copyToUnitServer()));
        connect(menuMoveTo, SIGNAL(triggered()), this, SLOT(moveToUnitServer()));
        menu->addAction(menuL);
        menu->addAction(menuUL);
        menu->addAction(menuI);
        menu->addAction(menuDI);
        menu->addAction(menuStart);
        menu->addAction(menuStop);
        menu->addAction(menuDuplicate);
        menu->addAction(menuCopyTo);
        menu->addAction(menuMoveTo);
        menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
    }
}

void UnitServerEditor::updateAll() {
    submitApiResult(TAG_GET_DESCRIPTION,
                    GET_CHAOS_API_PTR(unit_server::GetDescription)->execute(node_unique_id.toStdString()));
    //update instances
    handleSelectionChanged(ui->listViewCUType->selectionModel()->selection());
}

void UnitServerEditor::handleSelectionChanged(const QItemSelection& selection) {
    //remove all previously impl from the api container

    //add the new selected
    std::vector<std::string> control_unit_implementation;
    foreach (QModelIndex element, selection.indexes()) {
        QString impl = element.data().toString();
        qDebug() << "Add " << impl << " as filter for instances search";
        control_unit_implementation.push_back(impl.toStdString());
    }
    //resend the search
    submitApiResult(TAG_CU_SI,
                    GET_CHAOS_API_PTR(control_unit::SearchInstancesByUS)->execute(node_unique_id.toStdString(),
                                                                                  control_unit_implementation));
}

void UnitServerEditor::tableCurrentChanged(const QModelIndex &current,
                                           const QModelIndex &previous) {
    ui->pushButtonEditInstance->setEnabled(ui->tableView->selectionModel()->selectedRows().size() > 0);
}

void UnitServerEditor::tableSelectionChanged(const QItemSelection& selected,
                                             const QItemSelection& unselected) {
    ui->pushButtonRemoveCUType->setEnabled(ui->tableView->selectionModel()->selectedRows().size() > 0);
    ui->pushButtonEditInstance->setEnabled(ui->tableView->selectionModel()->selectedRows().size() > 0);
}


void UnitServerEditor::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "Received asyncApiResult event of tag:" << tag;
    if(tag.compare(TAG_GET_DESCRIPTION) == 0) {
        //uid
        ui->labelUnitServerUID->setText(node_unique_id);
        //address
        if(api_result->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
            //we have address
            ui->labelUinitServerAddress->setText(QString::fromStdString(api_result->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR)));
        } else {
            ui->labelUinitServerAddress->setText(tr("No address"));
        }

        if(api_result->hasKey(chaos::NodeDefinitionKey::NODE_TIMESTAMP)) {
            uint64_t ts = api_result->getUInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP);
            ui->labelRegistrationTimestamp->setText(QDateTime::fromMSecsSinceEpoch(ts, Qt::LocalTime).toString());
        } else {
            ui->labelRegistrationTimestamp->setText(tr("No registration timestamp found!"));
        }

        QStringList cy_type_list;
        if(api_result->hasKey(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS)) {
            //get the vector of unit type
            std::auto_ptr<CMultiTypeDataArrayWrapper> arr(api_result->getVectorValue(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS));
            for(int i = 0;
                i < arr->size();
                i++) {
                QString found_type = QString::fromStdString(arr->getStringElementAtIndex(i));
                qDebug() << "Found type:" << found_type << " for unit server:" << node_unique_id;
                cy_type_list.append(found_type);

            }
        }
        list_model_cu_type->setStringList(cy_type_list);
    } else if(tag.compare(TAG_CU_SI) == 0) {
        //whe have the result for the control unit searchs
        table_model->setRowCount(0);
        instance_list.clear();

        if(!api_result.isNull() && api_result->hasKey("node_search_result_page")) {
            CMultiTypeDataArrayWrapper *arr =  api_result->getVectorValue("node_search_result_page");
            for(int i = 0;
                i < arr->size();
                i++) {
                QSharedPointer<CDataWrapper> found_node(arr->getCDataWrapperElementAtIndex(i));
                instance_list.push_back(found_node);
                fillTableWithInstance(found_node);
            }
        }
    } else if(tag.compare("cu_load") == 0) {
        qDebug() << "Load sucessfull";
    } else if(tag.compare("cu_unload") == 0) {
        qDebug() << "unload sucessfull";
    } else if(tag.compare(TAG_CU_ADD_NEW_TYPE)==0) {
        updateAll();
    }else if(tag.compare(TAG_CU_REMOVE_TYPE_AND_UPDATE_LIST)==0) {
        updateAll();
    }
    PresenterWidget::onApiDone(tag,
                               api_result);
}

void UnitServerEditor::fillTableWithInstance( QSharedPointer<CDataWrapper> cu_instance) {
    QList<QStandardItem *> row_item;
    QStandardItem *item = NULL;
    row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(cu_instance, NodeDefinitionKey::NODE_UNIQUE_ID, "no found"))));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(cu_instance, "control_unit_implementation", "no found"))));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(cu_instance, NodeDefinitionKey::NODE_RPC_ADDR, "--nr--"))));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(cu_instance, NodeDefinitionKey::NODE_RPC_DOMAIN, "--nr--"))));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    table_model->appendRow(row_item);
}

void UnitServerEditor::on_pushButtonCreateNewInstance_clicked()
{
    //check if we have selected a type
    QModelIndexList selected_index = ui->listViewCUType->selectionModel()->selectedIndexes();
    if(!(selected_index.size() == 1)) {
        QMessageBox::information(this,
                                 tr("New Instance creation error"),
                                 tr("Instance creation needs a one control unit type selected!"));
        return;
    }
    //we can start instance editor
    addWidgetToPresenter(new ControUnitInstanceEditor(node_unique_id,
                                                      selected_index.first().data().toString()));
}

void UnitServerEditor::on_pushButtonUpdateAllInfo_clicked() {
    updateAll();
}

void UnitServerEditor::on_pushButtonRemoveInstance_clicked() {
    //add the new selected
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedIndexes()) {
        QString cu_inst_id = table_model->item(element.row(), 0)->text();
        qDebug() << "Remove " << cu_inst_id << " instance";
        submitApiResult(QString("cu_ri"),
                        GET_CHAOS_API_PTR(control_unit::DeleteInstance)->execute(node_unique_id.toStdString(),
                                                                                 cu_inst_id.toStdString()));
    }
    //update the result
    updateAll();
}

void UnitServerEditor::on_pushButtonEditInstance_clicked() {
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QString cu_inst_id = table_model->item(element.row(), 0)->text();
        qDebug() << "Edit " << cu_inst_id << " instance";
        addWidgetToPresenter(new ControUnitInstanceEditor(node_unique_id,
                                                          cu_inst_id,
                                                          true));
    }
}


void UnitServerEditor::on_pushButtonUpdateControlUnitType_clicked() {
    submitApiResult(TAG_GET_DESCRIPTION,
                    GET_CHAOS_API_PTR(unit_server::GetDescription)->execute(node_unique_id.toStdString()));
}


//------------------------------control unit slot------------------------
void UnitServerEditor::cuInstanceLoadSelected() {
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        qDebug() << "Send load message for " << QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        //load the selected cu
        submitApiResult(QString("cu_load"),
                        GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID),
                                                                                       true));
    }
}

void UnitServerEditor::cuInstanceUnloadSelected() {
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        qDebug() << "Send unload message for " << QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        //load the selected cu
        submitApiResult(QString("cu_unload"),
                        GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID),
                                                                                       false));
    }
}

void UnitServerEditor::cuInstanceInitSelected() {
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        qDebug() << "Send unload message for " << QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        //load the selected cu
        submitApiResult(QString("cu_init"),
                        GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID),
                                                                             true));
    }
}

void UnitServerEditor::cuInstanceDeinitSelected() {
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        qDebug() << "Send unload message for " << QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        //load the selected cu
        submitApiResult(QString("cu_deinit"),
                        GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID),
                                                                             false));
    }
}

void UnitServerEditor::cuInstanceStartSelected() {
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        qDebug() << "Send unload message for " << QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        //load the selected cu
        submitApiResult(QString("cu_start"),
                        GET_CHAOS_API_PTR(control_unit::StartStop)->execute(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID),
                                                                            true));
    }
}

void UnitServerEditor::cuInstanceStopSelected() {
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        qDebug() << "Send unload message for " << QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        //load the selected cu
        submitApiResult(QString("cu_start"),
                        GET_CHAOS_API_PTR(control_unit::StartStop)->execute(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID),
                                                                            false));
    }
}

void UnitServerEditor::duplicateInstance() {
    bool ok = false;
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        QString source_cu_id = QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        QString destination_cu_id = QInputDialog::getText(this,
                                                          (tr("Duplicate instance:")+source_cu_id),
                                                          tr("Unique ID destination:"),
                                                          QLineEdit::Normal,
                                                          tr(""), &ok);
        if (ok && !destination_cu_id.isEmpty()) {
            qDebug() << "Duplicate " << source_cu_id;
            //load the selected cu
            submitApiResult(QString("copy_instance"),
                            GET_CHAOS_API_PTR(control_unit::CopyInstance)->execute(source_cu_id.toStdString(),
                                                                                   node_unique_id.toStdString(),
                                                                                   destination_cu_id.toStdString(),
                                                                                   node_unique_id.toStdString()));
        } else if (!ok) {
            break;
        }
    }
}

void UnitServerEditor::moveToUnitServer() {
    if(ui->tableView->selectionModel()->selectedRows().size() == 0) return;
    if(move_copy_search_instance){
        move_copy_search_instance->show();
        return;
    }
    QString tag = "move<";
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        QString source_cu_id = QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        tag.append(source_cu_id);
        tag.append("<");
    }
    tag.resize(tag.size()-1);
    move_copy_search_instance = new SearchNodeResult(true, tag);
    connect(move_copy_search_instance, SIGNAL(selectedNodes(QString,QVector<QPair<QString,QString> >)), SLOT(selectedUnitServer(QString,QVector<QPair<QString,QString> >)));
}

void UnitServerEditor::copyToUnitServer() {
    if(ui->tableView->selectionModel()->selectedRows().size() == 0) return;
    if(move_copy_search_instance){
        move_copy_search_instance->show();
        return;
    }
    QString tag = "copy<";
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QSharedPointer<CDataWrapper> inst = instance_list[element.row()];
        QString source_cu_id = QString::fromStdString(inst->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        tag.append(source_cu_id);
        tag.append("<");
    }
    tag.resize(tag.size()-1);
    move_copy_search_instance = new SearchNodeResult(true, tag);
    connect(move_copy_search_instance, SIGNAL(selectedNodes(QString,QVector<QPair<QString,QString> >)), SLOT(selectedUnitServer(QString,QVector<QPair<QString,QString> >)));
    addWidgetToPresenter(move_copy_search_instance);
}

void UnitServerEditor::selectedUnitServer(const QString& tag, const QVector< QPair<QString,QString> >& selected_item) {

    int first_separator = tag.indexOf("<");
    QStringRef operation(&tag, 0, first_separator);
    QStringRef instances(&tag, first_separator+1, tag.size()-operation.size()-1);
    QVector<QStringRef> instance_to_apply = instances.split("<");

    foreach(QStringRef instance, instance_to_apply) {
        for(QVector<QPair<QString,QString> >::const_iterator it = selected_item.begin();
            it != selected_item.end();
            it++){
            if(operation.compare(tr("move")) == 0) {
            } else if(operation.compare(tr("copy"))==0){
                bool ok = false;
                QString cu_uid = instance.toString();
                QString us_uid = it->first;
                QString new_cu_uid = QString("%2/%1").arg(cu_uid, us_uid);
                QString destination_cu_id = QInputDialog::getText(this,
                                                                  (QString("Duplicate instance:%1 to unit server:%2").arg(cu_uid, us_uid)),
                                                                  tr("Unique ID destination:"),
                                                                  QLineEdit::Normal,
                                                                  new_cu_uid,
                                                                  &ok);
                if(ok && destination_cu_id.size()) {
                    //copy or move insnace to all target
                    submitApiResult(QString("copy_instance"),
                                    GET_CHAOS_API_PTR(control_unit::CopyInstance)->execute(instance.toString().toStdString(),
                                                                                           node_unique_id.toStdString(),
                                                                                           destination_cu_id.toStdString(),
                                                                                           it->first.toStdString()));
                }
            } else {
                break;
            }
        }
    }
    move_copy_search_instance->close();
    delete(move_copy_search_instance);
    move_copy_search_instance = NULL;
}

void UnitServerEditor::on_pushButtonAddNewCUType_clicked() {
    bool ok = false;
    QString cu_type = QInputDialog::getText(this,
                                            tr("Create new control unit type"),
                                            tr("Control Unit Type:"),
                                            QLineEdit::Normal,
                                            tr(""), &ok);
    if(ok && cu_type.size() > 0) {
        submitApiResult(TAG_CU_ADD_NEW_TYPE,
                        GET_CHAOS_API_PTR(unit_server::ManageCUType)->execute(node_unique_id.toStdString(),
                                                                              cu_type.toStdString(),
                                                                              0));
    }
}

void UnitServerEditor::on_pushButtonRemoveCUType_clicked() {
    int count = ui->listViewCUType->selectionModel()->selectedRows().size();
    QModelIndexList cu_type_list = ui->listViewCUType->selectionModel()->selectedRows();
    for(int idx = 0; idx < count; idx++) {
        QModelIndex cu_type = cu_type_list.at(idx);
        if(idx+1 < count) {
            submitApiResult(TAG_CU_REMOVE_TYPE,
                            GET_CHAOS_API_PTR(unit_server::ManageCUType)->execute(node_unique_id.toStdString(),
                                                                                  cu_type.data().toString().toStdString(),
                                                                                  1));
        } else {
            //we are at the last element and we need to udpate the list at the end of this call
            submitApiResult(TAG_CU_REMOVE_TYPE_AND_UPDATE_LIST,
                            GET_CHAOS_API_PTR(unit_server::ManageCUType)->execute(node_unique_id.toStdString(),
                                                                                  cu_type.data().toString().toStdString(),
                                                                                  1));
        }

    }
}

void UnitServerEditor::changedNodeOnlineStatus(const QString& node_uid,
                                               CLedIndicatorHealt::AliveState alive_state) {
    if(alive_state == CLedIndicatorHealt::Online) {
        updateAll();
    }
}

void UnitServerEditor::on_tableView_doubleClicked(const QModelIndex &index) {
    QStandardItem *node_uid = table_model->item(index.row(), 0);
    qDebug() << "Open control unit editor for" << node_uid->text();
    addWidgetToPresenter(new ControlUnitEditor(node_uid->text()));
}
