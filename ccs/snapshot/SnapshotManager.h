#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include "../presenter/PresenterWidget.h"
#include "SnapshotTableModel.h"
#include "NodeInSnapshotTableModel.h"
#include "../data/SnapshotDatasetNodeListModel.h"
#include "../data/LiveDatasetTableModel.h"

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class SnapshotManager;
}

class SnapshotManager :
        public PresenterWidget {
    Q_OBJECT
protected:
    void initUI();
    bool isClosing();
    //api has ben called successfully
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
public:
    explicit SnapshotManager(QWidget *parent = 0);
    ~SnapshotManager();

private slots:
    void on_pushButtonNewSnapshot_clicked();

    void on_pushButtonDeleteSnapshot_clicked();

    void on_pushButtonRestoreSnapshot_clicked();

    void tableSelectionChanged(const QItemSelection & from, const QItemSelection & to);
    void on_pushButtonSearchSnapshot_clicked();

    void on_listViewNodesInSnapshot_clicked(const QModelIndex &index);

    void on_listViewSnapshotNodeDataset_clicked(const QModelIndex &index);

private:
    void executeSearch();
    QString current_snapshot_name;
    SnapshotTableModel snapshot_table_model;
    NodeInSnapshotTableModel node_in_snapshot_list_model;
    //shanpstho node information
    SnapshotDatasetNodeListModel lm_dataset_for_node_snapshot;
    LiveDatasetTableModel tm_snapshot_dataset_view;
    Ui::SnapshotManager *ui;
};

#endif // SNAPSHOTMANAGER_H
