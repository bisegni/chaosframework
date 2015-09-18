#include "SearchNodeResult.h"
#include "ui_searchnoderesult.h"
#include "node/unit_server/UnitServerEditor.h"
#include "node/control_unit/ControlUnitEditor.h"

#include <QDebug>
#include <QPair>
#include <QMessageBox>
#include <QHeaderView>
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

SearchNodeResult::SearchNodeResult(bool _selection_mode,
                                   const QString &_tag) :
    PresenterWidget(NULL),
    current_page_length(200),
    selection_mode(_selection_mode),
    tag(_tag),
    ui(new Ui::SearchNodeResult) {
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    ui->lineEditSearchCriteria->setFocus();

    search_timer.setSingleShot(true);
    connect(&search_timer,
            SIGNAL(timeout()),
            SLOT(on_pushButtonStartSearch_clicked()));
}

SearchNodeResult::~SearchNodeResult()
{
    delete ui;
}

bool SearchNodeResult::isClosing() {
    return true;
}

void SearchNodeResult::initUI() {
    setTabTitle("Search Nodes");

    if(selection_mode){
        ui->pushButtonActionOnSelected->setText(tr("select"));
    } else {
        ui->pushButtonActionOnSelected->setText(tr("open editor"));
    }

    QStringList search_types;
    search_types << "All types" << "Unit server" << "Control unit";
    ui->comboBoxSearchType->addItems(search_types);

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

    //add contextual menu to result table
    registerWidgetForContextualMenu(ui->tableViewResult,
                                    NULL,
                                    true);
}

void SearchNodeResult::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "Received asyncApiResult event of tag:" << tag;

    if(tag.compare(tr("search_result"))==0) {

        //clear the model
        table_model->setRowCount(0);

        if(!api_result.isNull() &&
                api_result->hasKey("node_search_result_page") &&
                api_result->isVectorValue("node_search_result_page")) {
            //we have result
            std::auto_ptr<CMultiTypeDataArrayWrapper> arr(api_result->getVectorValue("node_search_result_page"));

            if(arr->size()) {
                //get first element seq
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
            }
        }else{
            qDebug() << "No data found";
        }
    }
}

void SearchNodeResult::on_tableViewResult_clicked(const QModelIndex &index)
{
    QModelIndexList indexes = ui->tableViewResult->selectionModel()->selectedIndexes();
    ui->pushButtonActionOnSelected->setEnabled(indexes.size()>0);
}

void SearchNodeResult::on_pushButtonStartSearch_clicked()
{
    //submit api
    submitApiResult("search_result",
                    GET_CHAOS_API_PTR(node::NodeSearch)->execute(ui->lineEditSearchCriteria->text().toStdString(),
                                                                 ui->comboBoxSearchType->currentIndex(),
                                                                 0,
                                                                 current_page_length));
}

void SearchNodeResult::on_pushButtonActionOnSelected_clicked() {
    QModelIndexList indexes = ui->tableViewResult->selectionModel()->selectedRows();
    foreach (QModelIndex index, indexes) {
        on_tableViewResult_doubleClicked(index);
    }
}

void SearchNodeResult::startHealtMonitorAction() {
    QModelIndexList indexes = ui->tableViewResult->selectionModel()->selectedRows();
    foreach (QModelIndex selected_index, indexes) {
        QStandardItem *node_uid = table_model->item(selected_index.row(), 0);
        QStandardItem *node_type = table_model->item(selected_index.row(), 1);
        if(node_type->text().compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT)||
                node_type->text().compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER)) {
            //we can register for healt this node
            addNodeToHealtMonitor(node_uid->text());
        }
    }
}

void SearchNodeResult::stopHealtMonitorAction() {
    QModelIndexList indexes = ui->tableViewResult->selectionModel()->selectedRows();
    foreach (QModelIndex selected_index, indexes) {
        QStandardItem *node_uid = table_model->item(selected_index.row(), 0);
        QStandardItem *node_type = table_model->item(selected_index.row(), 1);
        if(node_type->text().compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT)||
                node_type->text().compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER)) {
            //we can register for healt this node
            removeNodeToHealtMonitor(node_uid->text());
        }
    }
}

void SearchNodeResult::on_tableViewResult_doubleClicked(const QModelIndex &index) {
    if(selection_mode) {
        QVector<QPair<QString,QString> > selected_nodes;
        QStandardItem *node_uid = table_model->item(index.row(), 0);
        QStandardItem *node_type = table_model->item(index.row(), 1);
        //add the selectged element
        selected_nodes.push_back(qMakePair(node_uid->text(), node_type->text()));

        //emit signal
        emit  selectedNodes(tag,
                            selected_nodes);

        closeTab();
    } else {
        QStandardItem *node_uid = table_model->item(index.row(), 0);
        QStandardItem *node_type = table_model->item(index.row(), 1);
        if(node_type->text().compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
            qDebug() << "Open unit server editor for" << node_uid->text();
            addWidgetToPresenter(new UnitServerEditor(node_uid->text()));
        }else if(node_type->text().compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0) {
            qDebug() << "Open control unit editor for" << node_uid->text();
            addWidgetToPresenter(new ControlUnitEditor(node_uid->text()));
        }
    }
}

void SearchNodeResult::on_lineEditSearchCriteria_textEdited(const QString &search_string) {
    qDebug() << "on_lineEditSearchCriteria_textEdited:" << search_string;
    if(search_string.size()) {
        //restart timer
        search_timer.start(500);
    } else {
        //if no string we not perform the search
        search_timer.stop();
        table_model->setRowCount(0);
    }
}
