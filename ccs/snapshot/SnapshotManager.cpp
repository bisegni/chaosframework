#include "SnapshotManager.h"
#include "ui_SnapshotManager.h"
#include "NewSnapshot.h"

#include <QStandardItem>

static QString TAG_SEARCH_SNAPSHOT  = "t_a_ss";
static QString TAG_DELETE_SNAPSHOT  = "t_a_ds";
static QString TAG_RESTORE_SNAPSHOT = "t_a_rs";
static QString TAG_LOAD_NODE_IN_SNAPSHOT = "t_a_lnis";

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

SnapshotManager::SnapshotManager(QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::SnapshotManager) {
    ui->setupUi(this);
}

SnapshotManager::~SnapshotManager() {
    delete ui;
}

void SnapshotManager::initUI() {
    setWindowTitle(tr("Snapshot Manager"));
    ui->pushButtonDeleteSnapshot->setEnabled(false);
    ui->pushButtonRestoreSnapshot->setEnabled(false);

    ui->tableViewSnapshotList->setModel(&snapshot_table_model);
    ui->listViewNodesInSnapshot->setModel(&node_in_snapshot_list_model);

    ui->tableViewSnapshotList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->tableViewSnapshotList->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableSelectionChanged(QItemSelection,QItemSelection)));
    executeSearch();
}

bool SnapshotManager::isClosing() {
    return true;
}

void SnapshotManager::onApiDone(const QString& tag,
                                QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare(TAG_SEARCH_SNAPSHOT) == 0) {
        snapshot_table_model.updateSnapshotList(service::GetAllSnapshot::getHelper(api_result.data()));
        node_in_snapshot_list_model.reset();
    } else if(tag.compare(TAG_DELETE_SNAPSHOT) == 0) {
        executeSearch();
    } else if(tag.compare(TAG_LOAD_NODE_IN_SNAPSHOT) == 0) {
        node_in_snapshot_list_model.updateSnapshotList(service::GetNodesForSnapshot::getHelper(api_result.data()));
    }
    PresenterWidget::onApiDone(tag, api_result);
}


void SnapshotManager::executeSearch() {
    submitApiResult(TAG_SEARCH_SNAPSHOT,
                    GET_CHAOS_API_PTR(service::GetAllSnapshot)->execute());
}

void SnapshotManager::on_pushButtonNewSnapshot_clicked() {
    launchPresenterWidget(new NewSnapshot());
}

void SnapshotManager::on_pushButtonDeleteSnapshot_clicked() {
    QModelIndexList selected_snapshots = ui->tableViewSnapshotList->selectionModel()->selectedRows();
    foreach(QModelIndex snap, selected_snapshots) {
        QVariant snap_name_item = snapshot_table_model.data(snap);
        QString snap_name = snap_name_item.toString();
        submitApiResult(TAG_DELETE_SNAPSHOT,
                        GET_CHAOS_API_PTR(service::DeleteSnapshot)->execute(snap_name.toStdString()));
    }
}

void SnapshotManager::on_pushButtonRestoreSnapshot_clicked() {
    QModelIndexList selected_snapshots = ui->tableViewSnapshotList->selectionModel()->selectedRows();
    foreach(QModelIndex snap, selected_snapshots) {
        QVariant snap_name_item = snapshot_table_model.data(snap);
        QString snap_name = snap_name_item.toString();
        submitApiResult(TAG_RESTORE_SNAPSHOT,
                        GET_CHAOS_API_PTR(service::RestoreSnapshot)->execute(snap_name.toStdString()));
    }
}

void SnapshotManager::tableSelectionChanged(const QItemSelection & from, const QItemSelection & to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    ui->pushButtonDeleteSnapshot->setEnabled(ui->tableViewSnapshotList->selectionModel()->selectedRows().size());
    ui->pushButtonRestoreSnapshot->setEnabled(ui->tableViewSnapshotList->selectionModel()->selectedRows().size());
    //reset node list
    node_in_snapshot_list_model.reset();
    if(ui->tableViewSnapshotList->selectionModel()->selectedRows().size() == 1) {
        //load nodes for selection
       QModelIndex row_selected =  ui->tableViewSnapshotList->selectionModel()->selectedRows().first();
       QVariant snap_name_item = snapshot_table_model.data(row_selected);
       QString snap_name = snap_name_item.toString();
       submitApiResult(TAG_LOAD_NODE_IN_SNAPSHOT,
                       GET_CHAOS_API_PTR(service::GetNodesForSnapshot)->execute(snap_name.toStdString()));
    }
}

void SnapshotManager::on_pushButtonSearchSnapshot_clicked() {
    executeSearch();
}
