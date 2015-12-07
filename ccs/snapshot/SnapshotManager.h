#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include "../presenter/PresenterWidget.h"
#include "SnapshotTableModel.h"

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

private:
    void executeSearch();

    SnapshotTableModel snapshot_table_model;
    Ui::SnapshotManager *ui;
};

#endif // SNAPSHOTMANAGER_H
