#include "SearchNodeResult.h"
#include "ui_searchnoderesult.h"

#include <QDebug>
#include <QMessageBox>
#include <QHeaderView>
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

SearchNodeResult::SearchNodeResult(int _search_type,
                                   const QString &_search_criteria) :
    PresenterWidget(NULL),
    search_type(_search_type),
    search_criteria(_search_criteria),
    current_page(0),
    current_page_length(30),
    ui(new Ui::SearchNodeResult)
{
    ui->setupUi(this);

    //conenct the signal
    (void)connect(this, SIGNAL(startUpdateResult()),
                  this, SLOT(updateResult()),
                  Qt::QueuedConnection);

    //fetch the api porxy
    ns_proxy = ChaosMetadataServiceClient::getInstance()->getApiProxy<NodeSearch>();
}

SearchNodeResult::~SearchNodeResult()
{
    delete ui;
}

void SearchNodeResult::getSearchTypeAsString(QString& type_description) {
    switch (search_type) {
    case 0:
        type_description = "All type";
        break;
    case 1:
        type_description = "Unit server";
        break;
    case 2:
        type_description = "Control unit";
        break;
    default:
        break;
    }
}

bool SearchNodeResult::canClose() {
    return true;
}

void SearchNodeResult::updateUI() {
    setTabTitle("Search Result");
    QString search_type_desc;
    getSearchTypeAsString(search_type_desc);
    ui->labelType->setText(search_type_desc);
    ui->labelCriteria->setText(search_criteria);

    // Create a new model
    // QStandardItemModel(int rows, int columns, QObject * parent = 0)
    table_model = new QStandardItemModel(this);
    table_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Node Unique ID"));
    table_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Type"));

    // Attach the model to the view
    ui->tableViewResult->setModel(table_model);

    QHeaderView *headerView = ui->tableViewResult->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //finisch to configure table
    ui->tableViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewResult->setSelectionBehavior(QAbstractItemView::SelectRows);

    //signal to start theresult update
    emit(startUpdateResult());
}

void SearchNodeResult::updateResult() {
    qDebug() << "Update the search";
    //clear the table
    table_model->clear();

    //call mds api
    ApiProxyResult r = ns_proxy->execute(search_criteria.toStdString(),
                                         search_type,
                                         current_page*current_page_length,
                                         current_page_length);
    if(r->wait()) {
        if(r->getError()) {
            //QMessageBox::critical(this, r->getErrorDomain(), r->getErrorMessage());
        } else {
            if(r->getResult()) {
                //fetch data
                CMultiTypeDataArrayWrapper *arr =  r->getResult()->getVectorValue("node_search_result_page");
                for(int i = 0;
                    i < arr->size();
                    i++) {
                    auto_ptr<CDataWrapper> found_node(arr->getCDataWrapperElementAtIndex(i));

                    QList<QStandardItem *> row_item;
                    row_item.append(new QStandardItem(QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID))));
                    row_item.append(new QStandardItem(QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE))));
                    table_model->appendRow(row_item);
                }

            }
        }
    }else{
        //QMessageBox::critical(this,"Error","No data received");
    }
}

void SearchNodeResult::on_pushButtonNextPage_clicked()
{
    qDebug() << "Fetch next page";
}

void SearchNodeResult::on_pushButtonPrevPage_clicked()
{
    qDebug() << "Fetch prev";
}

void SearchNodeResult::on_pushButtonOpenNodeInEditor_clicked()
{
    qDebug() << "Open selected in editor";
}
