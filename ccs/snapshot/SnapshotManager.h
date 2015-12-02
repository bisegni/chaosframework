#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include "../presenter/PresenterWidget.h"
#include "SnapshotTableModel.h"

#include <QWidget>

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

private:
    SnapshotTableModel snapshot_table_model;
    Ui::SnapshotManager *ui;
};

#endif // SNAPSHOTMANAGER_H
