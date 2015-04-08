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
    connect(ui->pushButtonUpdateUnitServerInfo, SIGNAL(clicked()), this, SLOT(updateUSInfo()));

    // Create a new model
    // QStandardItemModel(int rows, int columns, QObject * parent = 0)
    table_model = new QStandardItemModel(this);
    table_model->setHorizontalHeaderItem(0, new QStandardItem(QString("UID")));
    table_model->setHorizontalHeaderItem(1, new QStandardItem(QString("Type")));
    table_model->setHorizontalHeaderItem(2, new QStandardItem(QString("State")));
    table_model->setHorizontalHeaderItem(3, new QStandardItem(QString("State SM")));
    // Attach the model to the view
    ui->tableView->setModel(table_model);

    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //finisch to configure table
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //load info
    updateUSInfo();
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

void UnitServerEditor::updateUSInfo() {
    ApiProxyResult api_result = gnd_proxy->execute(node_unique_id.toStdString());
    submitApiResult(tr("gnd"),
                    api_result);
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
