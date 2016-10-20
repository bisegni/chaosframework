#include "ui_searchnoderesult.h"
#include "node/unit_server/UnitServerEditor.h"
#include "node/control_unit/ControlUnitEditor.h"
#include "../node/control_unit/ControlUnitEditor.h"
#include "../node/control_unit/ControUnitInstanceEditor.h"

#include <QDebug>
#include <QPair>
#include <QMap>
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>

const QString CM_EDIT_NODE      = "Edit Node";
const QString CM_EDIT_INSTANCE  = "Edit Instance";

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

}

SearchNodeResult::SearchNodeResult(bool _selection_mode,
                                   const QList<SearchNodeType>& _selectable_types,
                                   const QString& _tag):
    current_page_length(200),
    selection_mode(_selection_mode),
    tag(_tag),
    selectable_types(_selectable_types) {
    ui->setupUi(this);
}

SearchNodeResult::~SearchNodeResult() {
    delete ui;
}

bool SearchNodeResult::isClosing() {
    return true;
}

void SearchNodeResult::initUI() {
    setFocusPolicy(Qt::StrongFocus);
    ui->lineEditSearchCriteria->setFocus();

    search_timer.setSingleShot(true);
    connect(&search_timer,
            SIGNAL(timeout()),
            SLOT(on_pushButtonStartSearch_clicked()));

    setTabTitle("Search Nodes");

    if(selection_mode){
        ui->pushButtonActionOnSelected->setText(tr("select"));
    } else {
        ui->pushButtonActionOnSelected->setText(tr("open editor"));
    }

    QStringList search_types;
    if(selectable_types.size() == 0) {
        search_types << "All types" << "Unit server" << "Control unit";
    } else {
        foreach(SearchNodeType searchable_type , selectable_types) {
            switch(searchable_type) {
            case SNT_ALL_TYPE:
                search_types << "All types";
                break;
            case SNT_UNIT_SERVER:
                search_types << "Unit server";
                break;
            case SNT_CONTROL_UNIT:
                search_types << "Control unit";
                break;
            }
        }
    }
    ui->comboBoxSearchType->addItems(search_types);

    // Create a new model
    // QStandardItemModel(int rows, int columns, QObject * parent = 0)
    table_model = new QStandardItemModel(this);
    table_model->setHorizontalHeaderItem(0, new QStandardItem(QString("Node Unique ID")));
    table_model->setHorizontalHeaderItem(1, new QStandardItem(QString("Node type")));
    table_model->setHorizontalHeaderItem(2, new QStandardItem(QString("Heartbeat")));
    table_model->setHorizontalHeaderItem(3, new QStandardItem(QString("Health Status")));

    // Attach the model to the view
    ui->tableViewResult->setModel(table_model);

    QHeaderView *headerView = ui->tableViewResult->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //finisch to configure table
    ui->tableViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewResult->setSelectionBehavior(QAbstractItemView::SelectRows);

    //add contextual menu to result table
    QMap<QString, QVariant> cm_list_map;
    cm_list_map.insert(CM_EDIT_NODE, tr("Edit Node"));
    cm_list_map.insert(CM_EDIT_INSTANCE, tr("Edit Instance"));
    registerWidgetForContextualMenu(ui->tableViewResult,
                                    &cm_list_map,
                                    true);
}

void SearchNodeResult::contextualMenuActionTrigger(const QString& cm_title,
                                                   const QVariant& cm_data) {
    //call subcalss implementation
    PresenterWidget::contextualMenuActionTrigger(cm_title,
                                                 cm_data);
    if(cm_title.compare(CM_EDIT_NODE) == 0) {
        //edit node
        foreach (QModelIndex element, ui->tableViewResult->selectionModel()->selectedRows()) {
            QString unit_uid = table_model->item(element.row(), 0)->text();
            qDebug() << "Edit " << unit_uid << " instance";
            addWidgetToPresenter(new ControlUnitEditor(unit_uid));
        }
    } else if(cm_title.compare(CM_EDIT_INSTANCE) == 0) {
        //edit instance
        foreach (QModelIndex element, ui->tableViewResult->selectionModel()->selectedRows()) {
            QString unit_uid = table_model->item(element.row(), 0)->text();
            qDebug() << "Edit " << unit_uid << " instance";
            addWidgetToPresenter(new ControUnitInstanceEditor("",
                                                              unit_uid,
                                                              true));
        }
    }
}

void SearchNodeResult::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
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

                    if(found_node->hasKey("health_stat") &&
                            found_node->isCDataWrapperValue("health_stat")) {
                        std::auto_ptr<CDataWrapper> health_stat(found_node->getCSDataValue("health_stat"));

                        row_item.append(item = new QStandardItem(QDateTime::fromMSecsSinceEpoch(health_stat->getUInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP), Qt::LocalTime).toString()));
                        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

                        row_item.append(item = new QStandardItem(QString::fromStdString(health_stat->getStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS))));
                        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    } else {
                        row_item.append(item = new QStandardItem(QString::fromStdString("---")));
                        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

                        row_item.append(item = new QStandardItem(QString::fromStdString("---")));
                        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    }
                    table_model->appendRow(row_item);
                }
            }else{
                qDebug() << "No data found";
            }
        }
        PresenterWidget::onApiDone(tag, api_result);
    }
}

void SearchNodeResult::on_tableViewResult_clicked(const QModelIndex &index) {
    QModelIndexList indexes = ui->tableViewResult->selectionModel()->selectedIndexes();

    ui->pushButtonActionOnSelected->setEnabled(indexes.size()>0);
    contextualMenuActionSetVisible(ui->tableViewResult,
                                   CM_EDIT_NODE,
                                   indexes.size()>0);
    contextualMenuActionSetVisible(ui->tableViewResult,
                                   CM_EDIT_INSTANCE,
                                   indexes.size()>0);
}

void SearchNodeResult::on_pushButtonStartSearch_clicked()
{
    //submit api
    if(ui->lineEditSearchCriteria->text().size() >=3) {
        submitApiResult("search_result",
                        GET_CHAOS_API_PTR(node::NodeSearch)->execute(ui->lineEditSearchCriteria->text().toStdString(),
                                                                     ui->comboBoxSearchType->currentIndex(),
                                                                     ui->checkBoxAliveOnly->isChecked(),
                                                                     0,
                                                                     current_page_length));
    }
}

void SearchNodeResult::on_pushButtonActionOnSelected_clicked() {
    QModelIndexList indexes = ui->tableViewResult->selectionModel()->selectedRows();
    foreach (QModelIndex index, indexes) {
        on_tableViewResult_doubleClicked(index);
    }
}

void SearchNodeResult::startHealtMonitorAction() {
    ui->tableViewResult->setCursor(Qt::WaitCursor);
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
    ui->tableViewResult->setCursor(Qt::ArrowCursor);
}

void SearchNodeResult::stopHealtMonitorAction() {
    ui->tableViewResult->setCursor(Qt::WaitCursor);
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
    ui->tableViewResult->setCursor(Qt::ArrowCursor);
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
    if(search_string.size() >= 3) {
        //restart timer
        search_timer.start(500);
    } else {
        //if no string we not perform the search
        search_timer.stop();
        table_model->setRowCount(0);
    }
}
