#ifndef TREEGROUPMANAGER_H
#define TREEGROUPMANAGER_H

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
    explicit TreeGroupManager(bool _selection_mode = false,
                              const QString& _selection_tag = QString(),
                              QWidget *parent = 0);
    ~TreeGroupManager();
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
signals:
    void selectedPath(const QString& selection_tag,
                      const QStringList& selected_path);

private slots:
    void on_pushButtonUpdateDomainsList_clicked();

    void on_pushButtonAddNewDomain_clicked();

    void handleListSelectionChanged(const QItemSelection &current_selection,
                                    const QItemSelection &previous_selection);

    void handleTreeSelectionChanged(const QItemSelection & selected,
                                    const QItemSelection & deselected);

    void on_pushButtonAddRoot_clicked();

    void domainUpdated();

    void on_pushButtonAcceptSelection_clicked();
private:
    void contextualMenuActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);
    Ui::TreeGroupManager *ui;
    bool selection_mmode;
    QString selection_tag;
    GroupTreeModel tree_model;
    DomainListModel domain_list_model;
};

#endif // TREEGROUPMANAGER_H
