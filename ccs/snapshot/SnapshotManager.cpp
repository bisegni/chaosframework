#include "SnapshotManager.h"
#include "ui_SnapshotManager.h"
#include "NewSnapshot.h"
#include "../metatypes.h"
#include "../utility/MessageUtility.h"

#include <QStandardItem>

static QString TAG_SEARCH_SNAPSHOT  = "t_a_ss";
static QString TAG_DELETE_SNAPSHOT  = "t_a_ds";
static QString TAG_RESTORE_SNAPSHOT = "t_a_rs";
static QString TAG_LOAD_NODE_IN_SNAPSHOT = "t_a_lnis";

using namespace chaos::common::data;
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
    connect(ui->listViewNodesInSnapshot->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(listViewNodesInSnapshotSelectionChanged(QItemSelection,QItemSelection)));

    ui->tableViewSnapshotList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //snapshot dataset values visualization
    ui->listViewSnapshotNodeDataset->setModel(&lm_dataset_for_node_snapshot);
    connect(ui->listViewSnapshotNodeDataset->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(listViewSnapshotNodeDatasetSelectionChanged(QItemSelection,QItemSelection)));

    ui->tableViewSnappedDatasetAttributes->setModel(&tm_snapshot_dataset_view);

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
        snapshot_table_model.updateSnapshotList(std::move(service::GetAllSnapshot::getHelper(api_result.data())));
        node_in_snapshot_list_model.reset();
    } else if(tag.compare(TAG_DELETE_SNAPSHOT) == 0) {
        executeSearch();
    } else if(tag.compare(TAG_LOAD_NODE_IN_SNAPSHOT) == 0) {
        node_in_snapshot_list_model.updateSnapshotList(std::move(service::GetNodesForSnapshot::getHelper(api_result.data())));
    }
    PresenterWidget::onApiDone(tag, api_result);
}


void SnapshotManager::executeSearch() {
    submitApiResult(TAG_SEARCH_SNAPSHOT,
                    GET_CHAOS_API_PTR(service::GetAllSnapshot)->execute(ui->lineEdit->text().toStdString()));
}

void SnapshotManager::on_pushButtonNewSnapshot_clicked() {
    launchPresenterWidget(new NewSnapshot());
}

void SnapshotManager::on_pushButtonDeleteSnapshot_clicked() {
    QModelIndexList selected_snapshots = ui->tableViewSnapshotList->selectionModel()->selectedRows();
    foreach(QModelIndex snap, selected_snapshots) {
        QVariant snap_name_item = snapshot_table_model.data(snap);
        QString snap_name = snap_name_item.toString();

        if(MessageUtility::showYNDialog("Delete Operation",
                                        QString("Proceed with the delete operation of the %1 snapshot?").arg(snap_name))) {
            submitApiResult(TAG_DELETE_SNAPSHOT,
                            GET_CHAOS_API_PTR(service::DeleteSnapshot)->execute(snap_name.toStdString()));
        }
    }
}

void SnapshotManager::on_pushButtonRestoreSnapshot_clicked() {
    QModelIndexList selected_snapshots = ui->tableViewSnapshotList->selectionModel()->selectedRows();
    foreach(QModelIndex snap, selected_snapshots) {
        QVariant snap_name_item = snapshot_table_model.data(snap);
        QString snap_name = snap_name_item.toString();
        if(MessageUtility::showYNDialog("Restore Operation",
                                        QString("Proceed with the restore operation of the %1 snapshot?").arg(snap_name))) {
            submitApiResult(TAG_RESTORE_SNAPSHOT,
                            GET_CHAOS_API_PTR(service::RestoreSnapshot)->execute(snap_name.toStdString()));
        }
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
        //clear node and dataset for node
        lm_dataset_for_node_snapshot.clear();
        tm_snapshot_dataset_view.clear();
        //load nodes for selection
        QModelIndex row_selected =  ui->tableViewSnapshotList->selectionModel()->selectedRows().first();
        QVariant snap_name_item = snapshot_table_model.data(row_selected);
        current_snapshot_name = snap_name_item.toString();
        submitApiResult(TAG_LOAD_NODE_IN_SNAPSHOT,
                        GET_CHAOS_API_PTR(service::GetNodesForSnapshot)->execute(current_snapshot_name.toStdString()));
    }
}

void SnapshotManager::on_pushButtonSearchSnapshot_clicked() {
    executeSearch();
}

void SnapshotManager::listViewNodesInSnapshotSelectionChanged(const QItemSelection &selected,
                                                              const QItemSelection &deselected) {
    tm_snapshot_dataset_view.clear();
    lm_dataset_for_node_snapshot.clear();
    if(selected.indexes().size() == 0) return;
    const QString selected_node = selected.indexes().first().data().toString();
    lm_dataset_for_node_snapshot.updateDatasetListFor(selected_node,
                                                      current_snapshot_name);
}

void SnapshotManager::listViewSnapshotNodeDatasetSelectionChanged(const QItemSelection &selected,
                                                              const QItemSelection &deselected) {
    tm_snapshot_dataset_view.clear();
    if(selected.indexes().size()==0) return;
    CDWShrdPtr selected_dataset = selected.indexes().first().data(Qt::UserRole).value<CDWShrdPtr>();
    tm_snapshot_dataset_view.setDataset(selected_dataset);
}
