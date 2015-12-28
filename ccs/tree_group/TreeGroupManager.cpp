#include "TreeGroupManager.h"
#include "ui_TreeGroupManager.h"
#include "AddNewDomain.h"


#include <QMap>
#include <QDebug>
#include <QInputDialog>

//tree contextual menu label
static const QString TREE_CM_NEW_ROOT_GROUP = "Add New Root Group";
static const QString TREE_CM_NEW_SUB_GROUP = "Add New Sub Group";
static const QString TREE_CM_UPDATE_DOMAIN_CHILD = "Update Domain Child";

//list contextual menu label
static const QString LIST_CM_SELECT_DOMAIN = "Select Domain";


using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

TreeGroupManager::TreeGroupManager() :
    PresenterWidget(NULL),
    ui(new Ui::TreeGroupManager) {
    ui->setupUi(this);
}

TreeGroupManager::~TreeGroupManager() {
    delete ui;
}

void TreeGroupManager::initUI() {
    QMap<QString, QVariant> cm_tree_map;
    cm_tree_map.insert(TREE_CM_NEW_ROOT_GROUP, tr("new_root"));
    cm_tree_map.insert(TREE_CM_NEW_SUB_GROUP, tr("new_sub"));
    cm_tree_map.insert(TREE_CM_UPDATE_DOMAIN_CHILD, tr("update_child"));

    QMap<QString, QVariant> cm_list_map;
    cm_list_map.insert(LIST_CM_SELECT_DOMAIN, tr("select_domain"));

    registerWidgetForContextualMenu(ui->treeViewDomainsTree,
                                    &cm_tree_map,
                                    false);

    ui->listViewDomains->setModel(&domain_list_model);
    connect(ui->listViewDomains->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(handleListSelectionChanged(QModelIndex,QModelIndex)));

    registerWidgetForContextualMenu(ui->listViewDomains,
                                    &cm_list_map,
                                    false);

    enableWidgetAction(ui->treeViewDomainsTree,
                       TREE_CM_NEW_ROOT_GROUP,
                       true);
    enableWidgetAction(ui->treeViewDomainsTree,
                       TREE_CM_UPDATE_DOMAIN_CHILD,
                       true);

    ui->treeViewDomainsTree->setModel(&tree_model);
    connect(ui->treeViewDomainsTree->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleTreeSelectionChanged(QItemSelection,QItemSelection)));

    updateDomains();
}

bool TreeGroupManager::isClosing() {
    true;
}

void TreeGroupManager::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "onApiDone";
    PresenterWidget::onApiDone(tag,
                               api_result);

    if(tag.compare("get_domains") == 0) {
        domain_list_model.update(groups::GetDomains::getHelper(api_result.data()));
        tree_model.clear();
    } else if(tag.compare("add_domain") == 0) {
        updateDomains();
    }

}

void TreeGroupManager::updateDomains() {
    submitApiResult("get_domains",
                    GET_CHAOS_API_PTR(groups::GetDomains)->execute());
}

void TreeGroupManager::contextualMenuActionTrigger(const QString& cm_title,
                                                   const QVariant& cm_data){
    if(cm_title.compare(LIST_CM_SELECT_DOMAIN)==0) {
        ui->labelDomainSelected->setText(cm_data.toString());
        tree_model.loadRootsForDomain(cm_data.toString());
    } else if(cm_title.compare(TREE_CM_NEW_SUB_GROUP) == 0) {
        QModelIndex parent_node_index = cm_data.toModelIndex();
        GroupTreeItem *parent_item = static_cast<GroupTreeItem*>(parent_node_index.internalPointer());
        if(parent_item == NULL) return;
        bool ok = false;
        QString node_name = QInputDialog::getText(this,
                                                  tr("Create new subgroup "),
                                                  tr("Subgroup Name for\n%2:").arg(parent_item->getPathToRoot()),
                                                  QLineEdit::Normal,
                                                  tr("New Subgroup Name"),
                                                  &ok);
        if(ok && node_name.size()>0) {
            tree_model.addNewNodeToIndex(parent_node_index, node_name);
        }
    } else if(cm_title.compare(TREE_CM_UPDATE_DOMAIN_CHILD) == 0) {
        QModelIndex parent_node_index = cm_data.toModelIndex();
        GroupTreeItem *parent_item = static_cast<GroupTreeItem*>(parent_node_index.internalPointer());
        if(parent_item == NULL) return;
        tree_model.updateNodeChildList(parent_node_index);
    }
}

void TreeGroupManager::handleListSelectionChanged(const QModelIndex &current_row,
                                                  const QModelIndex &previous_row) {
    if(!current_row.isValid()) {
        enableWidgetAction(ui->listViewDomains,
                           LIST_CM_SELECT_DOMAIN,
                           false);
    } else {
        enableWidgetAction(ui->listViewDomains,
                           LIST_CM_SELECT_DOMAIN,
                           true);
        setWidgetActionData(ui->listViewDomains,
                            LIST_CM_SELECT_DOMAIN,
                            current_row.data());
    }
}

void TreeGroupManager::handleTreeSelectionChanged(const QItemSelection & selected,
                                                  const QItemSelection & deselected) {
    bool empty_selection = selected.indexes().size() == 0;
    bool single_selection = selected.indexes().size() == 1;
    bool can_create_child = single_selection;

    enableWidgetAction(ui->treeViewDomainsTree,
                       TREE_CM_NEW_SUB_GROUP,
                       can_create_child);
    enableWidgetAction(ui->treeViewDomainsTree,
                       TREE_CM_UPDATE_DOMAIN_CHILD,
                       can_create_child);

    if(can_create_child) {
        //set the model indel as contextual menu data
        setWidgetActionData(ui->treeViewDomainsTree,
                            TREE_CM_NEW_SUB_GROUP,
                            selected.indexes().first());
        //set the model indel as contextual menu data
        setWidgetActionData(ui->treeViewDomainsTree,
                            TREE_CM_UPDATE_DOMAIN_CHILD,
                            selected.indexes().first());
    }


}

void TreeGroupManager::on_pushButtonUpdateDomainsList_clicked() {
    updateDomains();
}

void TreeGroupManager::on_pushButton_clicked() {
    AddNewDomain add_new_domain_dialog;
    int result = add_new_domain_dialog.exec();
    if(result == 1) {
        //we can create the new domain with one root
        submitApiResult("add_domain",
                        GET_CHAOS_API_PTR(groups::AddNode)->execute(add_new_domain_dialog.getDomainName().toStdString(),
                                                                    add_new_domain_dialog.getRootName().toStdString()));
    }
}
