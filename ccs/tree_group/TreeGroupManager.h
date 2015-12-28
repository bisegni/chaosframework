#ifndef TREEGROUPMANAGER_H
#define TREEGROUPMANAGER_H

#include <QWidget>
#include <QItemSelection>

#include "DomainListModel.h"
#include "../presenter/PresenterWidget.h"
#include "GroupTreeModel.h"

namespace Ui {
class TreeGroupManager;
}

class TreeGroupManager :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit TreeGroupManager();
    ~TreeGroupManager();
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void updateDomains();
private slots:
    void on_pushButtonUpdateDomainsList_clicked();

    void on_pushButton_clicked();

    void handleListSelectionChanged(const QModelIndex& current_row,
                                    const QModelIndex& previous_row);

    void handleTreeSelectionChanged(const QItemSelection & selected,
                                    const QItemSelection & deselected);
private:
    void contextualMenuActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);
    Ui::TreeGroupManager *ui;

    GroupTreeModel tree_model;
    DomainListModel domain_list_model;
};

#endif // TREEGROUPMANAGER_H
