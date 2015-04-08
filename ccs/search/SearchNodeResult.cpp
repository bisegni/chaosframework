#include "SearchNodeResult.h"
#include "ui_searchnoderesult.h"
#include "node/unit_server/UnitServerEditor.h"

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

void SearchNodeResult::initUI() {
    setTabTitle("Search Result");
    QString search_type_desc;
    getSearchTypeAsString(search_type_desc);
    ui->labelType->setText(search_type_desc);
    ui->labelCriteria->setText(search_criteria);

    // Create a new model
    // QStandardItemModel(int rows, int columns, QObject * parent = 0)
    table_model = new QStandardItemModel(this);
    table_model->setHorizontalHeaderItem(0, new QStandardItem(QString("Node Unique ID")));
    table_model->setHorizontalHeaderItem(1, new QStandardItem(QString("Node type")));

    // Attach the model to the view
    ui->tableViewResult->setModel(table_model);

    QHeaderView *headerView = ui->tableViewResult->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //finisch to configure table
    ui->tableViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewResult->setSelectionBehavior(QAbstractItemView::SelectRows);

    //start the query
    ApiProxyResult api_result = ns_proxy->execute(search_criteria.toStdString(),
                                                  search_type,
                                                  current_page*current_page_length,
                                                  current_page_length);

    //submit api
    submitApiResult(QString::fromUtf8("search_result"),
                    api_result);
}

void SearchNodeResult::onApiDone(QString tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "Received asyncApiResult event of tag:" << tag;

    if(tag.compare(tr("search_result"))==0) {
        setEnabled(true);

        //clear the model
        table_model->clear();
        if(api_result->hasKey("node_search_result_page")) {
            //we have result
            CMultiTypeDataArrayWrapper *arr =  api_result->getVectorValue("node_search_result_page");
            for(int i = 0;
                i < arr->size();
                i++) {
                auto_ptr<CDataWrapper> found_node(arr->getCDataWrapperElementAtIndex(i));

                QList<QStandardItem *> row_item;
                QStandardItem *item = NULL;
                row_item.append(item = new QStandardItem(QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID))));
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

                row_item.append(item = new QStandardItem(QString::fromStdString(found_node->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE))));
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

                table_model->appendRow(row_item);
            }
        }else{
            qDebug() << "No data found";
        }
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
    QModelIndexList indexes = ui->tableViewResult->selectionModel()->selectedRows();
    foreach (QModelIndex index, indexes) {
        QStandardItem *node_uid = table_model->item(index.row(), 0);
        QStandardItem *node_type = table_model->item(index.row(), 1);
       if(node_type->text().compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
           qDebug() << "Open unit server editor for" << node_uid->text();
           addWidgetToPresenter(new UnitServerEditor(node_uid->text()));
       }
    }
}

void SearchNodeResult::on_tableViewResult_clicked(const QModelIndex &index)
{
    QModelIndexList indexes = ui->tableViewResult->selectionModel()->selectedIndexes();
    ui->pushButtonOpenNodeInEditor->setEnabled(indexes.size()>0);
}
