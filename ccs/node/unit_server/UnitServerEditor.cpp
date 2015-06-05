#include "UnitServerEditor.h"
#include "ui_UnitServerEditor.h"
#include "../control_unit/ControUnitInstanceEditor.h"
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
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

UnitServerEditor::UnitServerEditor(const QString &_node_unique_id) :
    PresenterWidget(NULL),
    node_unique_id(_node_unique_id),
    ui(new Ui::UnitServerEditor)
{
    ui->setupUi(this);
    ui->splitterTypeInstances->setStretchFactor(0,0);
    ui->splitterTypeInstances->setStretchFactor(1,1);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(customMenuRequested(QPoint)));
}

UnitServerEditor::~UnitServerEditor()
{
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

    connect(ui->tableView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(tableCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->tableView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableSelectionChanged(QItemSelection,QItemSelection)));

    //load info
    updateAll();
}

bool UnitServerEditor::canClose() {
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
        connect(menuL, SIGNAL(triggered()), this, SLOT(cuInstanceLoadSelected()));
        connect(menuUL, SIGNAL(triggered()), this, SLOT(cuInstanceUnloadSelected()));
        connect(menuI, SIGNAL(triggered()), this, SLOT(cuInstanceInitSelected()));
        connect(menuDI, SIGNAL(triggered()), this, SLOT(cuInstanceDeinitSelected()));
        connect(menuStart, SIGNAL(triggered()), this, SLOT(cuInstanceStartSelected()));
        connect(menuStop, SIGNAL(triggered()), this, SLOT(cuInstanceStopSelected()));
        menu->addAction(menuL);
        menu->addAction(menuUL);
        menu->addAction(menuI);
        menu->addAction(menuDI);
        menu->addAction(menuStart);
        menu->addAction(menuStop);
        menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
    }
}

void UnitServerEditor::updateAll() {
    submitApiResult(QString("get_description"),
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
    submitApiResult(QString("cu_si"),
                    GET_CHAOS_API_PTR(control_unit::SearchInstancesByUS)->execute(node_unique_id.toStdString(),
                                                                                  control_unit_implementation));
}

void UnitServerEditor::tableCurrentChanged(const QModelIndex &current,
                                           const QModelIndex &previous) {
    ui->pushButtonEditInstance->setEnabled(ui->tableView->selectionModel()->selectedRows().size() > 0);
}

void UnitServerEditor::tableSelectionChanged(const QItemSelection& selected,
                                             const QItemSelection& unselected) {
    ui->pushButtonEditInstance->setEnabled(ui->tableView->selectionModel()->selectedRows().size() > 0);
}


void UnitServerEditor::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "Received asyncApiResult event of tag:" << tag;
    if(tag.compare("get_description") == 0) {
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
    } else if(tag.compare("cu_si") == 0) {
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
    }
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
    if(!selected_index.size() == 1) {
        QMessageBox::information(this,
                                 tr("New Instance creation error"),
                                 tr("Instance creation needs a one control unit type selected!"));
        return;
    }
    //we can start instance editor
    addWidgetToPresenter(new ControUnitInstanceEditor(node_unique_id,
                                                      selected_index.first().data().toString()));
}

void UnitServerEditor::on_pushButtonUpdateAllInfo_clicked()
{
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
    submitApiResult(QString("get_description"),
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
    }}
