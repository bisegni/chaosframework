#include "SnapshotManager.h"
#include "ui_SnapshotManager.h"

#include <QStandardItem>

static QString TAG_SEARCH_SNAPSHOT= "t_ss";
static QString TAG_DELETE_SNAPSHOT= "t_ds";

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
        snapshot_table_model.updateSnapshotList(std::auto_ptr<service::GetAllSnapshotHelper>(service::GetAllSnapshot::getHelper(api_result.data())));
    }

    PresenterWidget::onApiDone(tag, api_result);
}


void SnapshotManager::executeSearch() {
    submitApiResult(TAG_SEARCH_SNAPSHOT,
                    GET_CHAOS_API_PTR(service::GetAllSnapshot)->execute());
}

void SnapshotManager::on_pushButtonNewSnapshot_clicked() {

}

void SnapshotManager::on_pushButtonDeleteSnapshot_clicked() {
    QModelIndexList selected_snapshots = ui->tableViewSnapshotList->selectionModel()->selectedRows();
    foreach(QModelIndex snap, selected_snapshots) {
        //QStandardItem *snap_name_item = snapshot_table_model.data(QModelIndex(snap.row(), 0));
        //QString snap_name = snap_name_item.data().toString();
        //submitApiResult(TAG_DELETE_SNAPSHOT,
                       // GET_CHAOS_API_PTR(service::DeleteSnapshot)->execute(snap_name.toStdString()));
    }
}

void SnapshotManager::on_pushButtonRestoreSnapshot_clicked() {

}

void SnapshotManager::tableSelectionChanged(const QItemSelection & from, const QItemSelection & to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    ui->pushButtonDeleteSnapshot->setEnabled(ui->tableViewSnapshotList->selectionModel()->selectedRows().size());
    ui->pushButtonRestoreSnapshot->setEnabled(ui->tableViewSnapshotList->selectionModel()->selectedRows().size());
}
