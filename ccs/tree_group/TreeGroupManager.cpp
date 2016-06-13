#include "TreeGroupManager.h"
#include "ui_TreeGroupManager.h"
#include "AddNewDomain.h"


#include <QMap>
#include <QDebug>
#include <QVector>
#include <QInputDialog>

//tree contextual menu label
static const QString TREE_CM_NEW_ROOT_GROUP = "Add Root Group";
static const QString TREE_CM_NEW_SUB_GROUP = "Add Group";
static const QString TREE_CM_DELETE_GROUP = "Delete Group";
static const QString TREE_CM_UPDATE_DOMAIN_CHILD = "Update Child";

//list contextual menu label
static const QString LIST_CM_SELECT_DOMAIN = "Select Domain";


using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

TreeGroupManager::TreeGroupManager(bool _selection_mode,
                                   QWidget *parent) :
    PresenterWidget(parent),
    selection_mmode(_selection_mode),
    ui(new Ui::TreeGroupManager) {
    ui->setupUi(this);
}

TreeGroupManager::~TreeGroupManager() {
    delete ui;
}

void TreeGroupManager::initUI() {
    //set for selection or not
    ui->pushButtonAcceptSelection->setVisible(selection_mmode);

    QMap<QString, QVariant> cm_tree_map;
    cm_tree_map.insert(TREE_CM_NEW_ROOT_GROUP, tr("new_root"));
    cm_tree_map.insert(TREE_CM_NEW_SUB_GROUP, tr("new_sub"));
    cm_tree_map.insert(TREE_CM_DELETE_GROUP, tr("delete_group"));
    cm_tree_map.insert(TREE_CM_UPDATE_DOMAIN_CHILD, tr("update_child"));

    QMap<QString, QVariant> cm_list_map;
    cm_list_map.insert(LIST_CM_SELECT_DOMAIN, tr("select_domain"));

    registerWidgetForContextualMenu(ui->treeViewDomainsTree,
                                    &cm_tree_map,
                                    false);

    ui->listViewDomains->setModel(&domain_list_model);
    connect(ui->listViewDomains->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleListSelectionChanged(QItemSelection,QItemSelection)));
    connect(&domain_list_model,
            SIGNAL(domainUpdated()),
            SLOT(domainUpdated()));

    registerWidgetForContextualMenu(ui->listViewDomains,
                                    &cm_list_map,
                                    false);

    contextualMenuActionSetVisible(ui->treeViewDomainsTree,
                                   TREE_CM_NEW_ROOT_GROUP,
                                   true);
    contextualMenuActionSetVisible(ui->treeViewDomainsTree,
                                   TREE_CM_NEW_SUB_GROUP,
                                   false);
    contextualMenuActionSetVisible(ui->treeViewDomainsTree,
                                   TREE_CM_UPDATE_DOMAIN_CHILD,
                                   false);
    contextualMenuActionSetVisible(ui->treeViewDomainsTree,
                                   TREE_CM_DELETE_GROUP,
                                   false);

    ui->treeViewDomainsTree->setModel(&tree_model);
    connect(ui->treeViewDomainsTree->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleTreeSelectionChanged(QItemSelection,QItemSelection)));

    domain_list_model.update();
}

bool TreeGroupManager::isClosing() {
    true;
}

void TreeGroupManager::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "onApiDone";
    PresenterWidget::onApiDone(tag,
                               api_result);

    if(tag.compare("add_domain") == 0) {
        domain_list_model.update();
    }
}

void TreeGroupManager::domainUpdated() {
    //reset tree
    tree_model.clear();
}

void TreeGroupManager::contextualMenuActionTrigger(const QString& cm_title,
                                                   const QVariant& cm_data){
    if(cm_title.compare(LIST_CM_SELECT_DOMAIN)==0) {
        ui->pushButtonAddRoot->setEnabled(true);
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
    } else if(cm_title.compare(TREE_CM_DELETE_GROUP) == 0) {
        QModelIndex parent_node_index = cm_data.toModelIndex();
        GroupTreeItem *parent_item = static_cast<GroupTreeItem*>(parent_node_index.internalPointer());
        if(parent_item == NULL) return;
        tree_model.deleteNode(parent_node_index);
    } else if(cm_title.compare(TREE_CM_NEW_ROOT_GROUP) == 0) {
        on_pushButtonAddRoot_clicked();
    }
}

void TreeGroupManager::handleListSelectionChanged(const QItemSelection &current_selection,
                                                  const QItemSelection &previous_selection) {

    ui->pushButtonDeleteDomain->setEnabled(current_selection.indexes().size());

    if(current_selection.indexes().size() == 1) {
        contextualMenuActionSetVisible(ui->listViewDomains,
                                       LIST_CM_SELECT_DOMAIN,
                                       true);
        contextualMenuActionSetData(ui->listViewDomains,
                                    LIST_CM_SELECT_DOMAIN,
                                    current_selection.first().indexes().first().data());
    } else {
        contextualMenuActionSetVisible(ui->listViewDomains,
                                       LIST_CM_SELECT_DOMAIN,
                                       false);
    }
}

void TreeGroupManager::handleTreeSelectionChanged(const QItemSelection & selected,
                                                  const QItemSelection & deselected) {
    bool empty_selection = selected.indexes().size() == 0;
    bool single_selection = selected.indexes().size() == 1;
    bool can_create_child = single_selection;

    if(selection_mmode){ui->pushButtonAcceptSelection->setEnabled(empty_selection == false);}

    contextualMenuActionSetVisible(ui->treeViewDomainsTree,
                                   TREE_CM_NEW_SUB_GROUP,
                                   can_create_child);
    contextualMenuActionSetVisible(ui->treeViewDomainsTree,
                                   TREE_CM_UPDATE_DOMAIN_CHILD,
                                   can_create_child);
    contextualMenuActionSetVisible(ui->treeViewDomainsTree,
                                   TREE_CM_DELETE_GROUP,
                                   can_create_child);
    if(can_create_child) {
        //set the model index as contextual menu data
        contextualMenuActionSetData(ui->treeViewDomainsTree,
                                    TREE_CM_NEW_SUB_GROUP,
                                    selected.indexes().first());

        contextualMenuActionSetData(ui->treeViewDomainsTree,
                                    TREE_CM_UPDATE_DOMAIN_CHILD,
                                    selected.indexes().first());

        contextualMenuActionSetData(ui->treeViewDomainsTree,
                                    TREE_CM_DELETE_GROUP,
                                    selected.indexes().first());
    }


}

void TreeGroupManager::on_pushButtonUpdateDomainsList_clicked() {
    domain_list_model.update();
}

void TreeGroupManager::on_pushButtonAddNewDomain_clicked() {
    AddNewDomain add_new_domain_dialog;
    int result = add_new_domain_dialog.exec();
    if(result == 1) {
        //we can create the new domain with one root
        submitApiResult("add_domain",
                        GET_CHAOS_API_PTR(groups::AddNode)->execute(add_new_domain_dialog.getDomainName().toStdString(),
                                                                    add_new_domain_dialog.getRootName().toStdString()));
    }
}

void TreeGroupManager::on_pushButtonAddRoot_clicked() {
    bool ok = false;
    QString root_node_name = QInputDialog::getText(this,
                                                   tr("Create new root"),
                                                   tr("Root node name:"),
                                                   QLineEdit::Normal,
                                                   tr("Root Name"),
                                                   &ok);
    if(ok && root_node_name.size()>0) {
        tree_model.addNewRoot(root_node_name);
    }
}

void TreeGroupManager::on_pushButtonAcceptSelection_clicked() {
    const QString domain_selected = tree_model.currentDomain();
    QStringList selected_path;

    foreach (QModelIndex current_index, ui->treeViewDomainsTree->selectionModel()->selectedRows()) {
        GroupTreeItem *current_item = static_cast<GroupTreeItem*>(current_index.internalPointer());
        if(current_item) {
            //add path
            selected_path << current_item->getPathToRoot();
        }
    }
    emit selectedPath(selected_path);
    closeTab();
}
