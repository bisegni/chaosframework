#include "ui_searchnoderesult.h"
#include "../metatypes.h"
#include "node/unit_server/UnitServerEditor.h"
#include "node/control_unit/ControlUnitEditor.h"
#include "../node/control_unit/ControlUnitEditor.h"
#include "../node/control_unit/ControUnitInstanceEditor.h"
#include "../data/delegate/TwoLineInformationListItemDelegate.h"

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
    current_page_length(1000),
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
    ui->lineEditSearchCriteria->setFocus();

    search_timer.setSingleShot(true);
    connect(&search_timer,
            SIGNAL(timeout()),
            SLOT(on_pushButtonStartSearch_clicked()));

    setTitle("Search Nodes");

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

    // Attach the model to the list
    ui->listViewResult->setModel(&search_list_model);
    ui->listViewResult->setItemDelegate(new TwoLineInformationListItemDelegate(ui->listViewResult));

    //add contextual menu to result table
    QMap<QString, QVariant> cm_list_map;
    cm_list_map.insert(CM_EDIT_NODE, tr("Edit Node"));
    cm_list_map.insert(CM_EDIT_INSTANCE, tr("Edit Instance"));
    registerWidgetForContextualMenu(ui->listViewResult,
                                    &cm_list_map,
                                    true);
}

void SearchNodeResult::contextualMenuActionTrigger(const QString& cm_title,
                                                   const QVariant& cm_data) {
    //call subcalss implementation
    PresenterWidget::contextualMenuActionTrigger(cm_title,
                                                 cm_data);
    foreach (QModelIndex element, ui->listViewResult->selectionModel()->selectedRows()) {
        QSharedPointer<TwoLineInformationItem> element_data = element.data().value< QSharedPointer<TwoLineInformationItem> >();

        if(cm_title.compare(CM_EDIT_NODE) == 0) {
            //edit node

            launchPresenterWidget(new ControlUnitEditor(element_data->title));
        } else if(cm_title.compare(CM_EDIT_INSTANCE) == 0) {
            launchPresenterWidget(new ControUnitInstanceEditor("",
                                                              element_data->title,
                                                              true));
        }
    }
}

void SearchNodeResult::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {

    PresenterWidget::onApiDone(tag, api_result);
}

void SearchNodeResult::on_listViewResult_clicked(const QModelIndex &index) {
    QModelIndexList indexes = ui->listViewResult->selectionModel()->selectedIndexes();

    ui->pushButtonActionOnSelected->setEnabled(indexes.size()>0);
    contextualMenuActionSetVisible(ui->listViewResult,
                                   CM_EDIT_NODE,
                                   indexes.size()>0);
    contextualMenuActionSetVisible(ui->listViewResult,
                                   CM_EDIT_INSTANCE,
                                   indexes.size()>0);
}

void SearchNodeResult::on_pushButtonStartSearch_clicked() {
    //perform search
    search_list_model.searchNode(ui->lineEditSearchCriteria->text(),
                                 ui->comboBoxSearchType->currentIndex(),
                                 ui->checkBoxAliveOnly->isChecked());
}

void SearchNodeResult::on_pushButtonActionOnSelected_clicked() {
    QModelIndexList indexes = ui->listViewResult->selectionModel()->selectedRows();
    foreach (QModelIndex index, indexes) {
        on_listViewResult_doubleClicked(index);
    }
}

void SearchNodeResult::on_listViewResult_doubleClicked(const QModelIndex &index) {
    QSharedPointer<TwoLineInformationItem> element_data = index.data().value< QSharedPointer<TwoLineInformationItem> >();
    QSharedPointer<CDataWrapper> chaos_description = element_data->data.value< QSharedPointer<CDataWrapper> >();
    const QString node_uid = QString::fromStdString(chaos_description->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    const QString node_type = QString::fromStdString(chaos_description->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE));

    if(selection_mode) {
        QVector<QPair<QString,QString> > selected_nodes;
        //add the selectged element
        selected_nodes.push_back(qMakePair(node_uid, node_type));
        //emit signal
        emit  selectedNodes(tag,
                            selected_nodes);

        closeTab();
    } else {
        if(node_type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
            qDebug() << "Open unit server editor for" << node_uid;
            launchPresenterWidget(new UnitServerEditor(node_uid));
        }else if(node_type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0) {
            qDebug() << "Open control unit editor for" << node_uid;
            launchPresenterWidget(new ControlUnitEditor(node_uid));
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
    }
}

void SearchNodeResult::startHealtMonitorAction() {
    ui->listViewResult->setCursor(Qt::WaitCursor);
    QModelIndexList indexes = ui->listViewResult->selectionModel()->selectedRows();
    foreach (QModelIndex selected_index, indexes) {
        QSharedPointer<TwoLineInformationItem> element_data = selected_index.data().value< QSharedPointer<TwoLineInformationItem> >();
        QSharedPointer<CDataWrapper> chaos_description = element_data->data.value< QSharedPointer<CDataWrapper> >();
        const QString node_uid = QString::fromStdString(chaos_description->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        const QString node_type = QString::fromStdString(chaos_description->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE));
        if(node_type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT)||
                node_type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER)) {
            //we can register for healt this node
            addNodeToHealtMonitor(node_uid);
        }
    }
    ui->listViewResult->setCursor(Qt::ArrowCursor);
}

void SearchNodeResult::stopHealtMonitorAction() {
    ui->listViewResult->setCursor(Qt::WaitCursor);
    QModelIndexList indexes = ui->listViewResult->selectionModel()->selectedRows();
    foreach (QModelIndex selected_index, indexes) {
        QSharedPointer<TwoLineInformationItem> element_data = selected_index.data().value< QSharedPointer<TwoLineInformationItem> >();
        QSharedPointer<CDataWrapper> chaos_description = element_data->data.value< QSharedPointer<CDataWrapper> >();
        const QString node_uid = QString::fromStdString(chaos_description->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
        const QString node_type = QString::fromStdString(chaos_description->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE));
        if(node_type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT)||
                node_type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER)) {
            //we can register for healt this node
            removeNodeToHealtMonitor(node_uid);
        }
    }
    ui->listViewResult->setCursor(Qt::ArrowCursor);
}

