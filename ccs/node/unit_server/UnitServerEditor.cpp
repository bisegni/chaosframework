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

using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

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

    gnd_proxy = ChaosMetadataServiceClient::getInstance()->getApiProxy<GetNodeDescription>();
    cu_si_proxy = ChaosMetadataServiceClient::getInstance()->getApiProxy<SearchInstancesByUS>();
    cu_si_proxy->unit_server_uid = _node_unique_id.toStdString();

    cu_di_proxy = ChaosMetadataServiceClient::getInstance()->getApiProxy<DeleteInstance>();
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
    ui->listViewCUType->setSelectionMode(QAbstractItemView::MultiSelection);

    //connect update button to interla slot
    connect(ui->pushButtonUpdateAllInfo, SIGNAL(clicked()), this, SLOT(updateAll()));

    // Create a new model
    // QStandardItemModel(int rows, int columns, QObject * parent = 0)
    table_model = new QStandardItemModel(this);
    table_model->setHorizontalHeaderItem(0, new QStandardItem(QString("UID")));
    table_model->setHorizontalHeaderItem(1, new QStandardItem(QString("Implementation")));
    table_model->setHorizontalHeaderItem(2, new QStandardItem(QString("State")));
    table_model->setHorizontalHeaderItem(3, new QStandardItem(QString("State SM")));

    // Attach the model to the view
    ui->tableView->setModel(table_model);

    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //finisch to configure table
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

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
    QModelIndex index=ui->tableView->indexAt(pos);

    QMenu *menu=new QMenu(this);
    QAction *menuL = new QAction("Load", this);
    QAction *menuUL = new QAction("Unload", this);
    QAction *menuI = new QAction("Init", this);
    QAction *menuDI = new QAction("Deinit", this);
    QAction *menuStart = new QAction("Start", this);
    QAction *menuStop = new QAction("Stop", this);
    menu->addAction(menuL);
    menu->addAction(menuUL);
    menu->addAction(menuI);
    menu->addAction(menuDI);
    menu->addAction(menuStart);
    menu->addAction(menuStop);
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void UnitServerEditor::updateAll() {
    submitApiResult(QString("gnd"),
                    gnd_proxy->execute(node_unique_id.toStdString()));
    submitApiResult(QString("cu_si"),
                    cu_si_proxy->execute(0, 100));
}

void UnitServerEditor::handleSelectionChanged(const QItemSelection& selection) {
    assert(cu_si_proxy);
    //remove all previously impl from the api container
    cu_si_proxy->control_unit_implementation.clear();

    //add the new selected
    foreach (QModelIndex element, selection.indexes()) {
        QString impl = element.data().toString();
        qDebug() << "Add " << impl << " as filter for instances search";
        cu_si_proxy->control_unit_implementation.push_back(impl.toStdString());
    }
    //resend the search
    submitApiResult(QString("cu_si"),
                    cu_si_proxy->execute());
}

void UnitServerEditor::tableCurrentChanged(const QModelIndex &current,
                                           const QModelIndex &previous) {
    ui->pushButtonEditInstance->setEnabled(ui->tableView->selectionModel()->selectedRows().size() > 0);
}

void UnitServerEditor::tableSelectionChanged(const QItemSelection& selected,
                                             const QItemSelection& unselected) {
    ui->pushButtonEditInstance->setEnabled(ui->tableView->selectionModel()->selectedRows().size() > 0);
}


void UnitServerEditor::onApiDone(QString tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "Received asyncApiResult event of tag:" << tag;
    if(tag.compare("gnd") == 0) {
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
            QDateTime timestamp;
            timestamp.setTime_t(ts);
            ui->labelRegistrationTimestamp->setText(timestamp.toString(Qt::SystemLocaleLongDate));
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
        if(!api_result.isNull() && api_result->hasKey("node_search_result_page")) {
            CMultiTypeDataArrayWrapper *arr =  api_result->getVectorValue("node_search_result_page");
            for(int i = 0;
                i < arr->size();
                i++) {
                auto_ptr<CDataWrapper> found_node(arr->getCDataWrapperElementAtIndex(i));

                QList<QStandardItem *> row_item;
                QStandardItem *item = NULL;
                row_item.append(item = new QStandardItem(QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID))));
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

                row_item.append(item = new QStandardItem(QString::fromStdString(found_node->getStringValue("control_unit_implementation"))));
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

                row_item.append(item = new QStandardItem(QString::fromStdString("---")));
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

                row_item.append(item = new QStandardItem(QString::fromStdString("---")));
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

                table_model->appendRow(row_item);
            }
        }
    }
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

void UnitServerEditor::on_pushButtonRemoveInstance_clicked()
{
    assert(cu_di_proxy);
    //add the new selected
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedIndexes()) {
        QString cu_inst_id = table_model->item(element.row(), 0)->text();
        qDebug() << "Remove " << cu_inst_id << " instance";
        submitApiResult(QString("cu_ri"),
                        cu_di_proxy->execute(node_unique_id.toStdString(),
                                             cu_inst_id.toStdString()));
    }
    //update the result
    updateAll();
}

void UnitServerEditor::on_pushButtonEditInstance_clicked()
{
    foreach (QModelIndex element, ui->tableView->selectionModel()->selectedRows()) {
        QString cu_inst_id = table_model->item(element.row(), 0)->text();
        qDebug() << "Edit " << cu_inst_id << " instance";
        addWidgetToPresenter(new ControUnitInstanceEditor(node_unique_id,
                                                          cu_inst_id,
                                                          true));
    }
}
