#include "DataServiceEditor.h"
#include "ui_DataServiceEditor.h"
#include "CreateEditDataService.h"
#include "../../search/SearchNodeResult.h"

#include <QMenu>
#include <QDebug>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

#define COMPOSE_TAG_FOR_LOAD_ASSOCIATION(ds) QString("la|%1").arg(ds)
#define GETDS_FROM_TAG_FOR_LOAD_ASSOCIATION(tag) (tag.split("|").size()==2?tag.split("|")[1]:QString::fromStdString("no-ds"))

DataServiceEditor::DataServiceEditor() :
    PresenterWidget(NULL),
    ui(new Ui::DataServiceEditor)
{
    ui->setupUi(this);
}

DataServiceEditor::~DataServiceEditor()
{
    delete ui;
}

void DataServiceEditor::initUI() {
    table_model_ds = new QStandardItemModel(this);
    table_model_ds->setHorizontalHeaderItem(0, new QStandardItem(tr("Unique Identifier")));
    table_model_ds->setHorizontalHeaderItem(1, new QStandardItem(tr("DirectIO Address")));
    table_model_ds->setHorizontalHeaderItem(2, new QStandardItem(tr("Endpoint")));
    // Attach the model to the view
    ui->tableViewDataServices->setModel(table_model_ds);
    ui->tableViewDataServices->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    table_model_acu = new QStandardItemModel(this);
    table_model_acu->setHorizontalHeaderItem(0, new QStandardItem(tr("Unique Identifier")));
    table_model_acu->setHorizontalHeaderItem(1, new QStandardItem(tr("RPC Address")));
    table_model_acu->setHorizontalHeaderItem(2, new QStandardItem(tr("RPC Domain")));
    table_model_acu->setHorizontalHeaderItem(3, new QStandardItem(tr("State")));
    table_model_acu->setHorizontalHeaderItem(4, new QStandardItem(tr("State SM")));
    // Attach the model to the view
    ui->tableViewAssociatedControlUnits->setModel(table_model_acu);
    ui->tableViewAssociatedControlUnits->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QPalette dataset_asscoiaiton_label_cp;
    dataset_asscoiaiton_label_cp.setColor(QPalette::Text, Qt::blue);
    ui->labelSelectedDataServiceForAssociationTable->setPalette(dataset_asscoiaiton_label_cp);

    //widget connection initialization
    //table data soruce
    connect(ui->tableViewDataServices->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(tableDSCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->tableViewDataServices->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableDSSelectionChanged(QItemSelection,QItemSelection)));

    ui->tableViewDataServices->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableViewDataServices,
            SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(tableDSCustomMenuRequested(QPoint)));

    //table association
    connect(ui->tableViewAssociatedControlUnits->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(tableAssocCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->tableViewAssociatedControlUnits->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableAssocSelectionChanged(QItemSelection,QItemSelection)));

    //load all dataservice
    submitApiResult(QString::fromUtf8("ds_get_all_ds"),
                    GET_CHAOS_API_PTR(data_service::GetAllDS)->execute());
}

bool DataServiceEditor::isClosing() {
    return true;
}

//-----------------ui event-----------
void DataServiceEditor::on_pushButtonAddDataService_clicked()
{
    //add new data service description
    CreateEditDataService new_data_service;
    connect(&new_data_service,
            SIGNAL(newDataService(QString,QString,int)),
            SLOT(handleNewDataService(QString,QString,int)));
    new_data_service.exec();
}
void DataServiceEditor::on_pushButtoneditDataService_clicked()
{
    //edit data service description
    QModelIndex to_edit = ui->tableViewDataServices->selectionModel()->selectedRows().first();

    CreateEditDataService edit_data_service(to_edit.row(),
                                            table_model_ds->item(to_edit.row(), 0)->text(),
                                            table_model_ds->item(to_edit.row(), 1)->text(),
                                            table_model_ds->item(to_edit.row(), 2)->text().toInt());
    connect(&edit_data_service,
            SIGNAL(updateDataService(int,QString,QString,int)),
            SLOT(handleUpdateDataService(int,QString,QString,int)));
    edit_data_service.exec();
}


void DataServiceEditor::on_pushButtonUpdateDSTable_clicked() {
    submitApiResult(QString::fromUtf8("ds_get_all_ds"),
                    GET_CHAOS_API_PTR(data_service::GetAllDS)->execute());
}

void DataServiceEditor::on_pushButtonRemoveDataService_clicked()
{
    //edit data service description
    QModelIndexList to_edit = ui->tableViewDataServices->selectionModel()->selectedRows();
    foreach ( QModelIndex index, to_edit) {
        submitApiResult(QString::fromUtf8("ds_delete_ds"),
                        GET_CHAOS_API_PTR(data_service::DeleteDS)->execute(table_model_ds->item(index.row(), 0)->text().toStdString()));
    }
}


void DataServiceEditor::on_pushButtonAssociateNewControlUnit_clicked() {
    QModelIndex ds_where_attach = ui->tableViewDataServices->selectionModel()->selectedRows().first();
    SearchNodeResult *search_node = new SearchNodeResult(true, table_model_ds->item(ds_where_attach.row(),0)->text());
    //register for emitted signal
    connect(search_node,
            SIGNAL(selectedNodes(QString, QVector<QPair<QString,QString> >)),
            SLOT(handleSelectedNodes(QString, QVector<QPair<QString,QString> >)));
    //show the search panel
    addWidgetToPresenter(search_node);
}


void DataServiceEditor::on_pushButtonRemoveAssociatedControlUnit_clicked() {
    QModelIndex     ds_where_attach = ui->tableViewDataServices->selectionModel()->selectedRows().first();
    QModelIndexList cu_to_detach = ui->tableViewAssociatedControlUnits->selectionModel()->selectedRows();
    //set the data for association
    std::vector<std::string> node_to_detach;
    foreach(QModelIndex index, cu_to_detach){
        node_to_detach.push_back(table_model_acu->item(index.row(), 0)->text().toStdString());
    }
    submitApiResult(QString::fromUtf8("delete_association"),
                    GET_CHAOS_API_PTR(data_service::AssociationControlUnit)->execute(table_model_ds->item(ds_where_attach.row(),0)->text().toStdString(),
                                                                                     node_to_detach,
                                                                                     false));
}


void DataServiceEditor::on_pushButtonUpdateAssociationList_clicked() {
    if(ui->labelSelectedDataServiceForAssociationTable->text().size()==0) return;
    //start the api for load all association ofr dataservice
    QString ds_selected = ui->labelSelectedDataServiceForAssociationTable->text();
    submitApiResult(COMPOSE_TAG_FOR_LOAD_ASSOCIATION(ds_selected),
                    GET_CHAOS_API_PTR(data_service::GetAssociationByDS)->execute(ds_selected.toStdString()));
}

void DataServiceEditor::handleSelectedNodes(const QString& tag,
                                            const QVector<QPair<QString,QString> >& selected_nodes) {
    //set the data for association
    std::vector<std::string> node_to_associate;
    //scann all ndoe and keep all control unit type
    for(QVector<QPair<QString,QString> >::const_iterator it = selected_nodes.begin();
        it != selected_nodes.end();
        it++){
        if(it->second.compare(NodeType::NODE_TYPE_CONTROL_UNIT) == 0) {
            //we can add this node
            node_to_associate.push_back(it->first.toStdString());
        }
    }
    submitApiResult(QString::fromUtf8("new_association"),
                    GET_CHAOS_API_PTR(data_service::AssociationControlUnit)->execute(tag.toStdString(),
                                                                                     node_to_associate,
                                                                                     true));
}

void DataServiceEditor::handleNewDataService(const QString& unique_id,
                                             const QString& direct_io_address,
                                             int endpoint) {
    submitApiResult(QString::fromUtf8("ds_new"),
                    GET_CHAOS_API_PTR(data_service::NewDS)->execute(unique_id.toStdString(),
                                                                    direct_io_address.toStdString(),
                                                                    endpoint));

}

void DataServiceEditor::handleUpdateDataService(int current_row,
                                                const QString& unique_id,
                                                const QString& direct_io_address,
                                                int endpoint) {
    submitApiResult(QString::fromUtf8("ds_update"),
                    GET_CHAOS_API_PTR(data_service::UpdateDS)->execute(unique_id.toStdString(),
                                                                       direct_io_address.toStdString(),
                                                                       endpoint));
}

//--------------table event------------------
void DataServiceEditor::tableDSCustomMenuRequested(QPoint pos) {
    //compose contexual menu
    QModelIndexList selected = ui->tableViewDataServices->selectionModel()->selectedRows();
    if(selected.size() == 1) {
        QMenu *menu=new QMenu(this);
        QAction *menuLoadAssociationForDS = new QAction("Load Association", this);
        connect(menuLoadAssociationForDS,
                SIGNAL(triggered()),
                SLOT(loadCUAssociationForDataService()));
        menu->addAction(menuLoadAssociationForDS);
        //show contextual menu
        menu->popup(ui->tableViewDataServices->viewport()->mapToGlobal(pos));
    }
}

void DataServiceEditor::loadCUAssociationForDataService() {
    QModelIndexList selected = ui->tableViewDataServices->selectionModel()->selectedRows();
    if(selected.size()!=1) return;
    QString ds_selected = table_model_ds->item(selected.first().row(),0)->text();

    submitApiResult(COMPOSE_TAG_FOR_LOAD_ASSOCIATION(ds_selected),
                    GET_CHAOS_API_PTR(data_service::GetAssociationByDS)->execute(ds_selected.toStdString()));
}

void DataServiceEditor::tableDSCurrentChanged(const QModelIndex &current,
                                              const QModelIndex &previous) {
    bool edit_enable = ui->tableViewDataServices->selectionModel()->selectedRows().size() == 1;
    bool remove_enable = ui->tableViewDataServices->selectionModel()->selectedRows().size() >= 1;
    ui->pushButtoneditDataService->setEnabled(edit_enable);
    ui->pushButtonAssociateNewControlUnit->setEnabled(edit_enable);
    ui->pushButtonRemoveDataService->setEnabled(remove_enable);
}

void DataServiceEditor::tableDSSelectionChanged(const QItemSelection& selected,
                                                const QItemSelection& unselected) {
    bool edit_enable = ui->tableViewDataServices->selectionModel()->selectedRows().size() == 1;
    bool remove_enable = ui->tableViewDataServices->selectionModel()->selectedRows().size() >= 1;
    ui->pushButtoneditDataService->setEnabled(edit_enable);
    ui->pushButtonAssociateNewControlUnit->setEnabled(edit_enable);
    ui->pushButtonRemoveDataService->setEnabled(remove_enable);
}

void DataServiceEditor::tableAssocCurrentChanged(const QModelIndex &current,
                                                 const QModelIndex &previous) {
    bool delete_enable = ui->tableViewDataServices->selectionModel()->selectedRows().size() >= 1;
    ui->pushButtonRemoveAssociatedControlUnit->setEnabled(delete_enable);
}

void DataServiceEditor::tableAssocSelectionChanged(const QItemSelection& selected,
                                                   const QItemSelection& unselected) {
    bool delete_enable = ui->tableViewDataServices->selectionModel()->selectedRows().size() >= 1;
    ui->pushButtonRemoveAssociatedControlUnit->setEnabled(delete_enable);
}
//-------------api envent---------------------
void DataServiceEditor::onApiDone(const QString& tag,
                                  QSharedPointer<CDataWrapper> api_result) {
    if(tag.compare("ds_new")==0) {
        showInformation(tr("Data Service"),
                        tr("Save"),
                        tr("The new data service has been created"));
        //reload all dataservice
        submitApiResult(QString::fromUtf8("ds_get_all_ds"),
                        GET_CHAOS_API_PTR(data_service::GetAllDS)->execute());
    } else if(tag.compare("ds_update")==0) {
        showInformation(tr("Data Service"),
                        tr("Update"),
                        tr("The data service has been updated"));
        //reload all dataservice
        submitApiResult(QString::fromUtf8("ds_get_all_ds"),
                        GET_CHAOS_API_PTR(data_service::GetAllDS)->execute());
    } else if(tag.compare("ds_delete_ds")==0){
        //reload all dataservice
        submitApiResult(QString::fromUtf8("ds_get_all_ds"),
                        GET_CHAOS_API_PTR(data_service::GetAllDS)->execute());
    } else if(tag.compare("ds_get_all_ds")==0) {
        fillTableDS(api_result);
    } else if((tag.compare("new_association")==0)||
              (tag.compare("delete_association")==0)){
        //udate the list
        on_pushButtonUpdateAssociationList_clicked();
    }else if(tag.startsWith("la|")) {
        //we have a result for the lsit of the association to a data service
        fillDataServiceAssocaition(GETDS_FROM_TAG_FOR_LOAD_ASSOCIATION(tag), api_result);
    }
    PresenterWidget::onApiDone(tag,
                               api_result);
}
void DataServiceEditor::fillDataServiceAssocaition(const QString& data_service,
                                                   QSharedPointer<CDataWrapper> api_result) {
    table_model_acu->setRowCount(0);
    ui->labelSelectedDataServiceForAssociationTable->setText(data_service);
    ui->pushButtonUpdateAssociationList->setEnabled(data_service.size()>0);
    if(!api_result.isNull() &&
            api_result->hasKey("node_search_result_page")) {
        std::auto_ptr<CMultiTypeDataArrayWrapper> arr(api_result->getVectorValue("node_search_result_page"));
        for(int i = 0;
            i < arr->size();
            i++) {
            QSharedPointer<CDataWrapper> found_ds(arr->getCDataWrapperElementAtIndex(i));
            QList<QStandardItem *> row_item;
            QStandardItem *item = NULL;
            row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(found_ds, NodeDefinitionKey::NODE_UNIQUE_ID, "never setuped"))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(found_ds, NodeDefinitionKey::NODE_RPC_ADDR, "never registered"))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(found_ds, NodeDefinitionKey::NODE_RPC_DOMAIN, "never registered"))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            row_item.append(item = new QStandardItem(tr("---")));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            row_item.append(item = new QStandardItem(tr("---")));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table_model_acu->appendRow(row_item);
        }
    }
}

void DataServiceEditor::fillTableDS(QSharedPointer<CDataWrapper> api_result) {
    table_model_ds->setRowCount(0);
    if(!api_result.isNull() &&
            api_result->hasKey("node_search_result_page")) {
        std::auto_ptr<CMultiTypeDataArrayWrapper> arr(api_result->getVectorValue("node_search_result_page"));;
        for(int i = 0;
            i < arr->size();
            i++) {
            QSharedPointer<CDataWrapper> found_ds(arr->getCDataWrapperElementAtIndex(i));
            QList<QStandardItem *> row_item;
            QStandardItem *item = NULL;
            row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(found_ds, NodeDefinitionKey::NODE_UNIQUE_ID, "no found"))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            row_item.append(item = new QStandardItem(QString::fromStdString(CHK_STR_AND_GET(found_ds, NodeDefinitionKey::NODE_DIRECT_IO_ADDR, "no configured"))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            row_item.append(item = new QStandardItem(QString::number(CHK_UINT32_AND_GET(found_ds, DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT, -1))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            table_model_ds->appendRow(row_item);
        }
    }
}
